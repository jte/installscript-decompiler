#include "IScript.h"
#include "Prototype.h"
#include "InternalPrototype.h"
#include "DLLPrototype.h"
#include "ActionFile.h"
#include "Action.h"
#include "FuncCallAction.h"
#include "FuncPrologAction.h"
#include "EndFuncAction.h"
#include <iostream>

CIScript::CIScript(const std::vector<uint8_t>& script) :
	m_script(script), m_streamPtr(m_script), m_functionId(-1)
{
	Read();
}

void CIScript::ReadHeader()
{
	if (m_script.size() < sizeof(m_header))
	{
		throw std::runtime_error("Header too small");
	}

	std::memcpy(&m_header, &m_script[0], sizeof(m_header));
	m_streamPtr = StreamPtr(m_script);
}

void CIScript::DecryptBuffer(uint32_t* seed, std::vector<uint8_t> buffer, char key)
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
	*seed += buffer.size();
}

void CIScript::PrintPrototypes()
{
	for (const auto& p : m_fns)
	{
		std::cout << *(p.prototype);
	}
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

void CIScript::ReadBBs(uint32_t tableOffset)
{
	StreamPtr ptr(m_script, tableOffset);
	Function* fn = nullptr;
	std::vector<CAction*> acts;
	for (size_t i = 0; i < m_header.numBBs; ++i)
	{
		// Read BB addrs from table
		uint32_t bbAddr = 0;
		ptr.Read(bbAddr);

		// Read actions at BB addr
		StreamPtr aptr(m_script, bbAddr);
		uint16_t numActions = 0;
		aptr.Read(numActions);
		
		for (size_t j = 0; j < numActions; ++j)
		{
			uint16_t actionId = 0;
			aptr.Read(actionId, false);
			auto newAct = CAction::FindFactory(actionId, this, aptr);
			if (CFuncPrologAction* funcProlog = dynamic_cast<CFuncPrologAction*>(newAct))
			{
				fn = &GetFnByBBId(i);
				fn->dataDeclList = funcProlog->GetDataDeclList();
			}
			else if (dynamic_cast<CEndFuncAction*>(newAct))
			{
				fn->bbs.push_back(acts);
				acts.clear();
				fn = nullptr;
			}
			else
			{
				acts.push_back(newAct);
			}
		}
		if (fn) 
		{
			fn->bbs.push_back(acts);
			acts.clear();
		}
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
	std::cout << "variant table " << std::endl;
	for (size_t i = 0; i < count; ++i)
	{
		// ref: DataDeclList::AddToListSymFlags
		uint16_t first;
		table.Read(first);
		uint16_t second;
		table.Read(second);
		std::cout << "variant entry " << i << ". first:" << first << ";second:" << second << std::endl;
	}
}

void CIScript::ReadSymFlagTable(StreamPtr& table)
{
	// use datadecllist
	uint16_t count = 0;
	table.Read(count);
	uint16_t unknown = 0;
	table.Read(unknown);
	std::cout << "sym flags table " << std::endl;
	for (size_t i = 0; i < count; ++i)
	{
		uint16_t first;
		table.Read(first);
		uint16_t second;
		table.Read(second);
		std::cout << "symflags entry " << i << ". first:" << first << ";second:" << std::hex << second << std::dec << std::endl;
	}
}

void CIScript::Read()
{
	ReadHeader();
	
	std::cout << "field_74 -> " << m_header.field_74 << std::endl;
	ReadExternTable(m_header.ExternTableOffset);
	StreamPtr pv(m_script, m_header.VariantTableOffset);
	ReadVariantTable(pv);
	StreamPtr psf(m_script, m_header.List4Offset);
	ReadSymFlagTable(psf);
	//ReadVariablesTable(externp);
	std::cout << ">.." << std::endl;
	ReadStructs(m_header.TypedefsTableOffset);
	ReadPrototypes(m_header.PrototypesTableOffset);
	ReadBBs(m_header.BBsTableOffset);
}

std::ostream& operator<<(std::ostream& out, const CIScript& o)
{
	size_t i = 0;
	for (const auto& fn : o.m_fns)
	{
		out << *(fn.prototype);
		for (const auto& bbx : fn.bbs)
		{
			out << "label_" << std::dec << i << std::hex << ":\n";
			++i;
			for (const auto& bb : bbx)
				out << *bb << '\n';
		}
		out << "\n\n" << std::endl;
	}
	return out;
}