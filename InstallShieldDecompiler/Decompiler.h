#pragma once

#include "Statement.h"
#include "Function.h"
#include <vector>
#include <ostream>
#include "IScript.h"

class CPrototype;

class CDecompiler
{
public:
	CDecompiler(const CIScript& script);
	CFunction& AddFunctionPrototype(CPrototype* proto);
	void AddGlobalVariable(CVariable* var);
	CVariable* GetGlobalVariable(const std::string& name);
	CVariable* GetGlobalVariable(ArgType type, int16_t address);
	CFunction& GetFunction(size_t address);
	friend std::ostream& operator<<(std::ostream& out, const CDecompiler& o);
protected:
	std::vector<CFunction> m_functions;
	std::vector<CVariable*> m_globalVars;
};