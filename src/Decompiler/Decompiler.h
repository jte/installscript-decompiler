#pragma once

#include "Function.h"
#include <vector>
#include <ostream>
#include <map>
#include "Variables/SymbolTable.h"
#include "DataDeclList.h"

class ScriptPrototype;
class CFrontend;

class CDecompiler
{
public:
	CDecompiler(/*const*/ CFrontend* script);
	CFunction& AddFunctionPrototype(ScriptPrototype* proto);
	CFunction& GetFunction(size_t address);
	friend std::ostream& operator<<(std::ostream& out, const CDecompiler& o);
protected:
	void AddGlobalVariables(const CDataDeclList& globals);
	std::vector<CFunction> m_functions;
private:
	CFrontend* m_frontend;
	SymbolTable m_globalVars;
};