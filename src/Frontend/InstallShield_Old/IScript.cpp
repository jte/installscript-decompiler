#include "IScript.h"
#include "Actions/Action.h"
#include "Actions/FuncPrologAction.h"
#include <iostream>
#include <string.h>
#include "Type.h"

namespace oldis
{

CIScript::CIScript(const std::vector<uint8_t>& script, HeaderKind hdrKind) :
	m_script(script), m_streamPtr(m_script), m_hdrKind(hdrKind)
{
	Read();
}

void CIScript::ReadHeader()
{
	m_streamPtr = StreamPtr(m_script);

	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
		break;
	case HeaderKind::aLuZ:
		break;
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::INS:
	{
		m_streamPtr.JumpTo(0xD);
		const std::string CopyrightString = m_streamPtr.ReadInsString();
		m_streamPtr.Read(m_numEvents);
		break;
	}
	case HeaderKind::Unrecognized:
		break;
	}
}

void CIScript::ReadGlobalStringTable(StreamPtr& ptr)
{
	uint16_t count = 0;
	std::vector<uint16_t> table;

	ptr.Read(count);
	ptr.ReadArray(table, count);
}

void CIScript::ReadLoadableStringTable(StreamPtr& ptr)
{
	uint16_t count;
	std::vector<SLoadableStringEntry> table;

	ptr.Read(count);
	for (size_t i = 0; i < count; i++)
	{
		SLoadableStringEntry entry;

		ptr.Read(entry.stringId);
		entry.name = ptr.ReadInsString();

		m_loadableStrings.push_back(entry);
	}
}

void CIScript::ReadLoadableNumberTable(StreamPtr& ptr)
{
	uint16_t count;
	std::vector<SLoadableNumberEntry> table;

	ptr.Read(count);
	for (size_t i = 0; i < count; i++)
	{
		SLoadableNumberEntry entry;

		ptr.Read(entry.numberId);
		entry.name = ptr.ReadInsString();

		m_loadableNumbers.push_back(entry);
	}
}

struct LocalPrototype
{
	uint8_t PrototypeType;
	uint8_t ReturnType;
	std::string DllName;
	std::string FunctionName;
	uint16_t EventIndex;
	uint16_t ParameterCount;
	std::vector<ArgumentTypeInfo> args;
};

ScriptPrototype* ToScriptPrototype(const LocalPrototype& proto, size_t funcId)
{
	ScriptPrototype* scriptProto = new ScriptPrototype;
	//scriptProto->SetFlags((uint8_t)proto->GetFlags());
	scriptProto->SetArguments(proto.args);
	if (proto.FunctionName.empty())
	{
		scriptProto->SetName("Func_" + std::to_string(funcId));
	}
	else
	{
		scriptProto->SetName(proto.FunctionName);
	}
	scriptProto->SetReturnType((ScriptType)proto.ReturnType);
	scriptProto->SetBBId(proto.EventIndex);

	/**auto dllProto = dynamic_cast<CDLLPrototype*>(proto);
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
	}*/

	return scriptProto;
}

void CIScript::ReadPrototypes(StreamPtr& ptr)
{
	uint16_t count;

	ptr.Read(count);

	for (size_t i = 0; i < count; i++)
	{
		LocalPrototype proto;

		ptr.Read(proto.PrototypeType);
		ptr.Read(proto.ReturnType);
		proto.DllName = ptr.ReadInsString();
		proto.FunctionName = ptr.ReadInsString();
		ptr.Read(proto.EventIndex);
		ptr.Read(proto.ParameterCount);

		for (size_t j = 0; j < proto.ParameterCount; j++)
		{
			uint16_t scriptType;
			uint16_t internalType;
			ArgumentTypeInfo arg;

			ptr.Read(scriptType);
			ptr.Read(internalType);
			arg.scriptType = (ScriptType)scriptType;
			arg.internalType = (ConcreteType)internalType;
			proto.args.push_back(arg);
		}
		
		m_fns.push_back(ScriptFunction(ToScriptPrototype(proto, i)));
	}
}

void CIScript::ReadStructs(StreamPtr& ptr)
{
	uint16_t count;

	ptr.Read(count);

	for (size_t i = 0; i < count; i++)
	{
		ScriptStruct scriptStruct;

		uint16_t packingNumber;
		std::string structName;
		uint16_t fieldsCount;

		ptr.Read(packingNumber);
		structName = ptr.ReadInsString();
		ptr.Read(fieldsCount);

		scriptStruct.SetName(structName);

		for (size_t j = 0; j < fieldsCount; j++)
		{
			uint16_t type;
			uint16_t size;
			std::string fieldName;

			ptr.Read(type);
			ptr.Read(size);
			fieldName = ptr.ReadInsString();

			// TODO: figure out elem count from size
			ScriptStruct::CStructMember member(fieldName, 0, (ScriptType)type);

			scriptStruct.AddMember(member);
		}
		m_structs.push_back(scriptStruct);
	}
}

void CIScript::ReadEvents(StreamPtr& ptr)
{
	ScriptFunction* fn = nullptr;
	size_t funcIdx = 0;

	for (size_t i = 0; i < m_numEvents; i++)
	{
		uint16_t Reserved;
		uint16_t numActions;
		ptr.Read(Reserved);
		ptr.Read(numActions);

		ISBasicBlock newBB;
		newBB.SetBBId(i);

		fn = &GetFnById(funcIdx);

		for (size_t j = 0; j < numActions; j++)
		{
			uint16_t actionId = 0;
			ptr.Read(actionId, false);
			auto newAct = CAction::FindFactory(actionId, this, ptr);

			if (CFuncPrologAction* funcProlog = dynamic_cast<CFuncPrologAction*>(newAct))
			{
				funcIdx++;
				fn = &GetFnById(funcIdx);
				fn->prototype->SetBBId(i);
				fn->dataDeclList = funcProlog->GetDataDeclList();
			}
			else
			{
				newBB.AddAction(newAct);
			}
		}

		if (fn)
			fn->bbs.push_back(newBB);
	}
}

const ScriptStruct* CIScript::GetStruct(size_t id) const
{
	return &m_structs.at(id);
}

void CIScript::Read()
{
	ReadHeader();

	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
		break;
	case HeaderKind::aLuZ:
		break;
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::Unrecognized:
		break;
	case HeaderKind::INS:
		// order matters
		ReadGlobalStringTable(m_streamPtr);
		ReadLoadableStringTable(m_streamPtr);
		m_streamPtr.Read(m_globalNumberCount);
		ReadLoadableNumberTable(m_streamPtr);
		ReadStructs(m_streamPtr);
		ReadPrototypes(m_streamPtr);
		ReadEvents(m_streamPtr);
		break;
	}
}

void CIScript::print(std::ostream& os) const
{
	for (auto s : m_structs)
	{
		os << s << std::endl;
	}
	os << std::endl;

	for (const auto& fn : m_fns)
	{
		if (fn.prototype->GetIsExported() || fn.bbs.size() != 0)
		{
			os << *(fn.prototype) << std::endl;
			os << fn.dataDeclList << std::endl;
			for (const auto& bb : fn.bbs)
			{
				for (const auto& act : bb.GetActions())
				{
					os << std::hex << bb.GetBBId() << std::dec << ": ";
					os << *act << std::endl;
				}
			}
			os << "\n\n" << std::endl;
		}
	}
}

};