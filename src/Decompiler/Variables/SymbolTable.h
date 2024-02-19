#pragma once

#include <vector>
#include <string>
#include "Variable.h"

class SymbolTable
{
public:
	SymbolTable() : m_parent(nullptr)
	{

	}
	CVariable* GetByAddress(int address, EVariableType type, bool isGlobal);
	CVariable* GetByName(const std::string& name, EVariableType type, bool isGlobal);
	void Add(CVariable* var);
	void SetParent(SymbolTable* parent)
	{
		m_parent = parent;
	}
	const std::vector<CVariable*>& GetVariables() const
	{
		return m_variables;
	}
protected:
private:
	SymbolTable* m_parent;
	std::vector<CVariable*> m_variables;
};