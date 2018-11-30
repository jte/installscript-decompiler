#include "Decompiler.h"
#include "InternalPrototype.h"
#include "Function.h"
#include "IScript.h"
#include "LabelStatement.h"
#include <algorithm>
#include "Action.h"

CDecompiler::CDecompiler(const CIScript& script)
{
	auto fns = script.GetFns();
	for (const auto& fn : fns)
	{
		auto outFn = AddFunctionPrototype(fn.prototype);
		for (const auto& bb : fn.bbs)
		{

		}
	}
/*	const auto& bbs = script.GetBasicBlocks();
	CFunction* currFunction = nullptr;
	size_t label = 0;
	for (auto bb = bbs.cbegin(); bb != bbs.cend(); ++bb)
	{
		size_t index = std::distance(bbs.cbegin(), bb);
		if (dynamic_cast<CFuncPrologAction*>((*bb)[0]))
		{
			currFunction = &optimizer.GetFunction(index);
			label = 0;
		}
		// detect func epilogue ?
		//currFunction->AddBasicBlock(TranslateBBs(label, *bb));
		label++;
	}*/
	for (const auto& e : script.GetExterns())
	{
		ArgType type = (ArgType)-1;
		if (e.type == 1)
			type = ArgType::VariantArg;
		else if (e.type == 2)
			;//??
		else if (e.type == 3)
			;//??
		CVariable* gvar = new CVariable(type, e.address, false);
		gvar->SetName(e.name);
		AddGlobalVariable(gvar);
	}
}

CFunction& CDecompiler::AddFunctionPrototype(CPrototype* proto)
{
	CInternalPrototype *iproto = dynamic_cast<CInternalPrototype*>(proto);
	if (iproto)
	{
		
		if (iproto->GetIsExported())
		{
			CFunction fn(iproto->GetName(), iproto->GetBBId(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		else
		{
			CFunction fn(iproto->GetBBId(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		CFunction& f = m_functions.back();
		f.SetArguments(iproto->GetArgumentTypeInfo());
		return f;
	}
	throw std::runtime_error("Invalid prototype");
}

void CDecompiler::AddGlobalVariable(CVariable* var)
{
	m_globalVars.push_back(var);
}

CVariable* CDecompiler::GetGlobalVariable(const std::string& name)
{
	auto found = std::find_if(m_globalVars.begin(), m_globalVars.end(), [&name](CVariable* var)
	{return var->GetName() == name; });
	return found != m_globalVars.end() ? *found : nullptr;
}

CVariable* CDecompiler::GetGlobalVariable(ArgType type, int16_t address)
{
	auto found = std::find_if(m_globalVars.begin(), m_globalVars.end(), [&address, &type](CVariable* var)
	{return var->GetAddress() == address && var->GetConcreteType() == type; });
	return found != m_globalVars.end() ? *found : nullptr;
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
	for (const auto &fn : o.m_functions)
	{
		out << fn << std::endl;
	}
	return out;
}