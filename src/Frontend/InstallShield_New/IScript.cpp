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
#include <string.h>
#include "ScriptPrototype.h"

namespace newis
{

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

		memcpy(&m_headerOBS, &m_script[0], sizeof(m_headerOBS));
		break;
	case HeaderKind::aLuZ:
		if (m_script.size() < sizeof(m_headerALUZ))
		{
			throw std::runtime_error("Header too small");
		}

		memcpy(&m_headerALUZ, &m_script[0], sizeof(m_headerALUZ));
		break;
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::Unrecognized:
		break;
	}

	m_streamPtr = StreamPtr(m_script);
}

ScriptPrototype* ToScriptPrototype(CPrototype* proto)
{
	ScriptPrototype* scriptProto = new ScriptPrototype;
	scriptProto->SetFlags((uint8_t)proto->GetFlags());
	scriptProto->SetArguments(proto->GetArgumentTypeInfo());
	scriptProto->SetName(proto->GetName());
	scriptProto->SetReturnType(proto->GetReturnType());
	scriptProto->SetBBId(proto->GetBBId());
	
	auto dllProto = dynamic_cast<CDLLPrototype*>(proto);
	if (dllProto)
	{
		auto name = dllProto->GetName();
		auto flags = dllProto->GetFlags();
		std::stringstream ss;
		if (((uint32_t)flags & (uint32_t)CPrototype::Flags::IsExported) || !name.empty())
			ss << dllProto->GetDLLName() << '.' << name;
		else
			ss << "Func_" << dllProto->GetBBId();

		scriptProto->SetName(ss.str());
	}

	auto internalProto = dynamic_cast<CInternalPrototype*>(proto);
	if (internalProto)
	{
		auto name = internalProto->GetName();
		auto flags = internalProto->GetFlags();
		std::stringstream ss;
		if (((uint32_t)flags & (uint32_t)CPrototype::Flags::IsExported) || !name.empty())
			ss << name;
		else
			ss << "Func_" << internalProto->GetBBId();
		
		scriptProto->SetName(ss.str());
	}

	return scriptProto;
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
			auto proto = new CPrototype(ptr);
			auto scriptProto = ToScriptPrototype(proto);
			m_fns.push_back(ScriptFunction(scriptProto));
		}
		else
		{
			CPrototype::MakeFn fn = CPrototype::FindFactory(flags);
			auto proto = (*fn)(ptr);
			auto scriptProto = ToScriptPrototype(proto);
			m_fns.push_back(ScriptFunction(scriptProto));
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
	ScriptFunction* fn = nullptr;

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
	ScriptFunction* fn = nullptr;
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

const CStruct* CIScript::GetStruct(size_t id) const
{
	return &m_structs.at(id);
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

void CIScript::Read()
{
	ReadHeader();

	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
	{
		ReadExternTable(m_headerOBS.ExternTableOffset);

		m_globalDeclList.ParseGlobalForOBS(m_script, m_headerOBS);

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
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::Unrecognized:
		break;
	}
}

void CIScript::print(std::ostream& os) const
{
	/**for (const auto& fn : o.m_fns)
	{
		if (fn.prototype->GetIsExported() || fn.bbs.size() != 0)
		{
			out << *(fn.prototype);
			out << fn.dataDeclList;
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
	}*/
}

};