#pragma once

#include "Function.h"
#include <vector>
#include <ostream>
#include "IScript.h"
#include <map>
#include "Variables/SymbolTable.h"

class CPrototype;

class CDecompiler
{
public:
	CDecompiler(const CIScript& script);
	CFunction& AddFunctionPrototype(CPrototype* proto);
	CFunction& GetFunction(size_t address);
	friend std::ostream& operator<<(std::ostream& out, const CDecompiler& o);
protected:
	void AddGlobalVariables(const CDataDeclList& globals);
	std::vector<CFunction> m_functions;
private:
	const CIScript& m_script;
	SymbolTable m_globalVars;
};