#include "IScript.h"
#include "Prototype/Prototype.h"
#include "Prototype/InternalPrototype.h"
#include "Prototype/DLLPrototype.h"
#include "ActionFile/ActionFile.h"
#include "Actions/Action.h"
#include "Actions/FuncCallAction.h"
#include "Actions/FuncPrologAction.h"
#include "Actions/EndFuncAction.h"
#include "Actions/IfAction.h"
#include "Actions/GotoAction.h"
#include <iostream>

CIScript::CIScript(const std::vector<uint8_t>& script, HeaderKind hdrKind) :
	m_script(script), m_streamPtr(m_script), m_hdrKind(hdrKind)
{
	Read();
}

void CIScript::ReadHeader()
{
	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
		if (m_script.size() < sizeof(m_headerOBS))
		{
			throw std::runtime_error("Header too small");
		}

		std::memcpy(&m_headerOBS, &m_script[0], sizeof(m_headerOBS));
		break;
	case HeaderKind::aLuZ:
		if (m_script.size() < sizeof(m_headerALUZ))
		{
			throw std::runtime_error("Header too small");
		}

		std::memcpy(&m_headerALUZ, &m_script[0], sizeof(m_headerALUZ));
		break;
	}

	m_streamPtr = StreamPtr(m_script);
}

void CIScript::DecryptBuffer(uint32_t* seed, std::vector<uint8_t>& buffer, uint8_t key)
{
	for (size_t i = 0; i < buffer.size(); ++i)
	{
		uint8_t v7 = ~key ^ ~buffer[i];
		uint8_t v8 = v7 >> 1;
		if (v7 & 1)
			v8 |= 0x80;
		uint8_t v9 = v8 >> 1;
		if (v8 & 1)
			v9 |= 0x80;
		buffer[i] = v9 - (i + *seed) % 0x47;
	}
	*seed += (uint32_t)buffer.size();
}

void CIScript::ReadPrototypes(uint32_t tableOffset)
{
	uint16_t numPrototypes = 0;
	StreamPtr ptr(m_script, tableOffset);
	ptr.Read(numPrototypes);

	for (size_t i = 0; i < numPrototypes; ++i)
	{
		uint8_t flags = 0;
		ptr.Read(flags, false);
		if ((flags & 4) == 4)
		{
			m_fns.push_back(Function(new CPrototype(ptr)));
		}
		else
		{
			CPrototype::MakeFn fn = CPrototype::FindFactory(flags);
			m_fns.push_back(Function((*fn)(ptr)));
		}
	}
}

void CIScript::ReadStructs(uint32_t tableOffset)
{
	uint16_t numStructs = 0;
	StreamPtr ptr(m_script, tableOffset);
	ptr.Read(numStructs);
	for (size_t i = 0; i < numStructs; ++i)
	{
		CStruct s(this);
		s.Parse(ptr);
		m_structs.push_back(s);
	}
}

void CIScript::ReadAddressResolve(uint32_t tableOffset)
{
	StreamPtr ptr(m_script, tableOffset);
	uint16_t numEntries = 0;
	ptr.Read(numEntries);
	for (size_t i = 0; i < numEntries; ++i)
	{
		uint8_t type = 0;
		uint32_t offset = 0;
		ptr.Read(type);
		ptr.Read(offset);
	}
}

void CIScript::ReadBBsOBS(uint32_t tableOffset)
{
	StreamPtr ptr(m_script, tableOffset);
	Function* fn = nullptr;

	for (size_t i = 0; i < m_headerOBS.numBBs; ++i)
	{
		// Read BB addrs from table
		uint32_t bbAddr = 0;
		ptr.Read(bbAddr);

		// Read actions at BB addr
		StreamPtr aptr(m_script, bbAddr);
		uint16_t numActions = 0;
		aptr.Read(numActions);

		ISBasicBlock newBb;
		newBb.SetBBId(bbAddr);

		for (size_t j = 0; j < numActions; ++j)
		{
			uint16_t actionId = 0;
			aptr.Read(actionId, false);
			auto newAct = CAction::FindFactory(actionId, this, aptr);
			newAct->SetBBId(bbAddr);
			if (CFuncPrologAction* funcProlog = dynamic_cast<CFuncPrologAction*>(newAct))
			{
				fn = &GetFnByBBId(i);
				fn->dataDeclList = funcProlog->GetDataDeclList();
			}
			else
			{
				newBb.AddAction(newAct);
			}
		}

		if (fn)
			fn->bbs.push_back(newBb);
	}
}

void CIScript::ReadBBsALUZ(uint32_t tableOffset)
{
	StreamPtr ptr(m_script, tableOffset);
	Function* fn = nullptr;
	uint16_t numBBs = 0;

	ptr.Read(numBBs);

	std::vector<uint32_t> bbAddrs;
	for (size_t i = 0; i < numBBs; i++)
	{
		uint32_t bbAddr = 0;
		ptr.Read(bbAddr);
		bbAddrs.push_back(bbAddr);
	}

	for (size_t i = 0; i < numBBs; i++)
	{
		StreamPtr bbPtr(m_script, bbAddrs[i]);
		uint16_t numActions = 0;
		bbPtr.Read(numActions);

		ISBasicBlock newBb;
		newBb.SetBBId(bbAddrs[i]);

		for (size_t j = 0; j < numActions; ++j)
		{
			uint16_t actionId = 0;
			bbPtr.Read(actionId, false);
			auto newAct = CAction::FindFactory(actionId, this, bbPtr);
			newAct->SetBBId(bbAddrs[i]);
			if (CFuncPrologAction* funcProlog = dynamic_cast<CFuncPrologAction*>(newAct))
			{
				fn = &GetFnByBBId(i);
				fn->dataDeclList = funcProlog->GetDataDeclList();
			}
			else
			{
				newBb.AddAction(newAct);
			}
		}

		if (fn)
			fn->bbs.push_back(newBb);
	}
}

const CStruct& CIScript::GetStruct(size_t id) const
{
	return m_structs.at(id);
}

void CIScript::ReadExternTable(uint32_t tableOffset)
{
	StreamPtr externTable(m_script, tableOffset);
	uint16_t count = 0;
	externTable.Read(count);
	for (size_t i = 0; i < count; ++i)
	{
		uint8_t type = 0;
		externTable.Read(type);
		int16_t address = 0;
		externTable.Read(address);
		std::string name = externTable.ReadInsString();

		m_externs.push_back(ExternRecord(type, name, address));
	}
}

void CIScript::ReadVariantTable(StreamPtr& table)
{
	// use datadecllist
	uint16_t count = 0;
	table.Read(count);
	//std::cout << "variant table " << std::endl;
	for (size_t i = 0; i < count; ++i)
	{
		// ref: DataDeclList::AddToListSymFlags
		uint16_t first;
		table.Read(first);
		uint16_t second;
		table.Read(second);
		//std::cout << "variant entry " << i << ". first:" << first << ";second:" << second << std::endl;
	}
}

void CIScript::ReadSymFlagTable(StreamPtr& table)
{
	// use datadecllist
	uint16_t count = 0;
	table.Read(count);
	uint16_t unknown = 0;
	table.Read(unknown);
	//std::cout << "sym flags table " << std::endl;
	for (size_t i = 0; i < count; ++i)
	{
		uint16_t first;
		table.Read(first);
		uint16_t second;
		table.Read(second);
		//std::cout << "symflags entry " << i << ". first:" << first << ";second:" << std::hex << second << std::dec << std::endl;
	}
}

void CIScript::Read()
{
	ReadHeader();
	
	switch (m_hdrKind)
	{
		case HeaderKind::OBS:
		{
			ReadExternTable(m_headerOBS.ExternTableOffset);

			StreamPtr pv(m_script, m_headerOBS.VariantTableOffset);
			ReadVariantTable(pv);
			StreamPtr psf(m_script, m_headerOBS.List4Offset);
			ReadSymFlagTable(psf);

			ReadStructs(m_headerOBS.TypedefsTableOffset);
			ReadPrototypes(m_headerOBS.PrototypesTableOffset);
			ReadBBsOBS(m_headerOBS.BBsTableOffset);
			//ReadAddressResolve(m_header.AddressResolveTableOffset);
			break;
		}
		case HeaderKind::aLuZ:
		{
			ReadStructs(m_headerALUZ.TypedefsTableOffset);
			StreamPtr pvars(m_script, m_headerALUZ.VariablesTableOffset);
			m_globalDeclList.Parse(pvars);
			ReadPrototypes(m_headerALUZ.PrototypesTableOffset);
			ReadBBsALUZ(m_headerALUZ.BBsTableOffset);
			break;
		}
	}
}

std::ostream& operator<<(std::ostream& out, const CIScript& o)
{
	size_t i = 0;
	for (const auto& fn : o.m_fns)
	{
		out << *(fn.prototype);
		for (const auto& bb : fn.bbs)
		{
			for (const auto& act : bb.GetActions())
			{
				out << std::hex << bb.GetBBId() << std::dec << ": ";
				out << *act << std::endl;
			}
		}
		out << "\n\n" << std::endl;
	}
	return out;
}