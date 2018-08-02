#include "Optimizer.h"
#include "InternalPrototype.h"
#include "Function.h"
#include <algorithm>

void COptimizer::AddFunctionPrototype(CPrototype* proto)
{
	CInternalPrototype *iproto = dynamic_cast<CInternalPrototype*>(proto);
	if (iproto)
	{
		
		if (iproto->GetIsExported())
		{
			CFunction fn(iproto->GetName(), iproto->GetAddress(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		else
		{
			CFunction fn(iproto->GetAddress(), iproto->GetReturnType());
			m_functions.push_back(fn);
		}
		CFunction& f = m_functions.back();
		f.SetArguments(iproto->GetArgumentTypeInfo());
	}
}

void COptimizer::AddGlobalVariable(CVariable* var)
{
	m_globalVars.push_back(var);
}

CVariable* COptimizer::GetGlobalVariable(const std::string& name)
{
	auto found = std::find_if(m_globalVars.begin(), m_globalVars.end(), [&name](CVariable* var)
	{return var->GetName() == name; });
	return found != m_globalVars.end() ? *found : nullptr;
}

CVariable* COptimizer::GetGlobalVariable(ArgType type, int16_t address)
{
	auto found = std::find_if(m_globalVars.begin(), m_globalVars.end(), [&address, &type](CVariable* var)
	{return var->GetAddress() == address && var->GetConcreteType() == type; });
	return found != m_globalVars.end() ? *found : nullptr;
}

CFunction& COptimizer::GetFunction(size_t address)
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

std::ostream& operator<<(std::ostream& out, const COptimizer& o)
{
	for (const auto &fn : o.m_functions)
	{
		out << fn << std::endl;
	}
	return out;
}