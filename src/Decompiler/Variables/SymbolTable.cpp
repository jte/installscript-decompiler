#include "SymbolTable.h"
#include "Variable.h"
#include <algorithm>

CVariable* SymbolTable::GetByAddress(int address, EVariableType type, bool isGlobal)
{
	auto found = std::find_if(m_variables.begin(), m_variables.end(), 
		[address, isGlobal, type](CVariable* var) {return var->GetAddress() == address && var->IsGlobal() == isGlobal && var->GetType() == type; });
	if (found != m_variables.end())
	{
		return *found;
	}
	else if (m_parent == nullptr)
	{
		throw std::runtime_error("GetByAddress couldn't find a variable with address: " + std::to_string(address));
		return nullptr;
	}
	else
	{
		return m_parent->GetByAddress(address, type, isGlobal);
	}
}

CVariable* SymbolTable::GetByName(const std::string& name, EVariableType type, bool isGlobal)
{
	auto found = std::find_if(m_variables.begin(), m_variables.end(), 
		[&name, isGlobal, type](CVariable* var) {return var->GetName() == name && var->IsGlobal() == isGlobal && var->GetType() == type; });
	if (found != m_variables.end())
	{
		return *found;
	}
	else if (m_parent == nullptr)
	{
		throw std::runtime_error("GetByAddress couldn't find a variable with name: " + name);
		return nullptr;
	}
	else
	{
		return m_parent->GetByName(name, type, isGlobal);
	}
}

void SymbolTable::Add(CVariable* var)
{
	m_variables.push_back(var);
}