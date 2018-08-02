#pragma once

#include "Statement.h"
#include "Function.h"
#include <vector>
#include <ostream>

class CPrototype;

class COptimizer
{
public:
	void AddFunctionPrototype(CPrototype* proto);
	void AddGlobalVariable(CVariable* var);
	CVariable* GetGlobalVariable(const std::string& name);
	CVariable* GetGlobalVariable(ArgType type, int16_t address);
	CFunction& GetFunction(size_t address); // AddFunctionPrototype must be run before this function is called
	friend std::ostream& operator<<(std::ostream& out, const COptimizer& o);
protected:
	std::vector<CFunction> m_functions;
	std::vector<CVariable*> m_globalVars;
};