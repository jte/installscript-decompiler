#include "Decompiler.h"
//#include "Prototype/InternalPrototype.h"
#include "Function.h"
#include "Frontend.h"
#include <algorithm>
#include <vector>
#include "Parser/Parser.h"
#include "Variables/NumberVariable.h"
#include "Variables/StringVariable.h"
#include "Variables/VariantVariable.h"
#include <optional>
#include "Frontend.h"
#include "ScriptPrototype.h"
#include "IRGen/IRGenerator.h"
#include "InstallShield_New/IScript.h"

CDecompiler::CDecompiler(CFrontend* frontend, const std::string cfgFile) : m_frontend(frontend), m_cfgFile(cfgFile)
{
	AddGlobalVariables(frontend->GetGlobalDeclList());
	auto fns = frontend->GetFns();
	for (const auto& fn : fns)
	{
		if (fn.bbs.size() == 0)
			continue; // skip predefined funcs
		auto& outFn = AddFunctionPrototype(fn.prototype);
		outFn.SetCfgFile(m_cfgFile);
		outFn.SetFrontend(frontend);
		outFn.SetGlobalSymTable(&m_globalVars);
		outFn.SetVariables(fn.dataDeclList);
		Parser parser;
		std::vector<AbstractExpression*> expressions = parser.Parse(fn.bbs, outFn.GetLocalVars());
		outFn.Construct(expressions);
	}
}

void CDecompiler::AddGlobalVariables(const CDataDeclList& globals)
{
	for (size_t i = 0; i < globals.GetNumNumbers(); i++)
	{
		m_globalVars.Add(new CNumberVariable(i));
	}

	for (size_t i = 0; i < globals.GetNumStrings(); i++)
	{
		std::optional<uint16_t> stringSize = std::nullopt;
		for (auto strInfo : globals.GetStringTable())
		{
			if (strInfo.varId == i)
			{
				stringSize = strInfo.stringSize;
			}
		}
		m_globalVars.Add(new CStringVariable(i, stringSize));
	}

	auto objectTable = globals.GetObjectTable();
	size_t variantId = 0;
	for (auto obj : objectTable)
	{
		CVariantVariable* var = new CVariantVariable(variantId);
		if (((int)obj.flags & (int)SymFlags::vbArray) != 0)
		{
			var->SetIsArray(true);
			var->SetElemCount(obj.elemCount);
		}
		else
		{
			if (obj.typedefId != -1)
			{
				var->SetIsStruct(true);
				var->SetTypedef(m_frontend->GetStruct(obj.typedefId));
			}
		}
		m_globalVars.Add(var);
		variantId++;
	}

	if (dynamic_cast<newis::CIScript*>(m_frontend))
	{
		m_globalVars.GetByAddress(0, EVariableType::Variant, true)->SetName("LAST_RESULT");
		m_globalVars.GetByAddress(1, EVariableType::Variant, true)->SetName("NOTHING");
		m_globalVars.GetByAddress(2, EVariableType::Variant, true)->SetName("Err");
	}
		
	for (auto extRecord : m_frontend->GetExterns())
	{
		switch (extRecord.GetType())
		{
			case EScriptExternType::Variant:
				m_globalVars.GetByAddress(extRecord.GetAddress(), EVariableType::Variant, true)->SetName(extRecord.GetName());
			break;
			case EScriptExternType::Number:
				m_globalVars.GetByAddress(extRecord.GetAddress(), EVariableType::Number, true)->SetName(extRecord.GetName());
			break;
			case EScriptExternType::String:
				m_globalVars.GetByAddress(extRecord.GetAddress(), EVariableType::String, true)->SetName(extRecord.GetName());
			break;
		}
	}
}

CFunction& CDecompiler::AddFunctionPrototype(ScriptPrototype* proto)
{
	if (proto->GetIsExported())
	{
		CFunction fn(proto->GetName(), proto->GetBBId(), proto->GetReturnType());
		m_functions.push_back(fn);
	}
	else
	{
		CFunction fn(proto->GetBBId(), proto->GetReturnType());
		m_functions.push_back(fn);
	}
	CFunction& f = m_functions.back();
	f.SetArguments(proto->GetArguments());
	return f;
}

CFunction& CDecompiler::GetFunction(size_t address)
{
	auto found = std::find_if(m_functions.begin(), m_functions.end(), [address] (CFunction& func) {
		return func.GetAddress() == address;
	});
	if (found == m_functions.end())
	{
		throw std::runtime_error("Could not find function prototype with address " + std::to_string(address));
	}
	return *found;
}

std::ostream& operator<<(std::ostream& out, const CDecompiler& o)
{
	for (auto s : o.m_frontend->GetStructs())
	{
		out << s << std::endl;
	}
	out << std::endl;
	
	for (auto global : o.m_globalVars.GetVariables())
	{
		out << *global << std::endl;
	}

	out << std::endl;
	for (const auto &fn : o.m_functions)
	{
		out << fn << std::endl;
	}

	return out;
}