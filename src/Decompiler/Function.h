#pragma once

#include "CFG/ControlFlowGraph.h"
#include "CFG/BasicBlock.h"
//#include "ISBasicBlock.h"
#include "IRGen/IRStatements.h"
#include "GraphVizPrinter.h"
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <iostream>
#include <stack>
#include "Parser/Expressions.h"
#include "Type.h"
#include "DataDeclList.h"
#include "Variables/SymbolTable.h"

class IRGenerator;
class SymbolTable;
class CFrontend;

class CFunction
{
public:
	CFunction(int32_t address, ScriptType returnType) :
		m_address(address), m_returnType(returnType), m_gen(nullptr), m_script(nullptr)
	{}
	CFunction(std::string name, int32_t address, ScriptType returnType) :
		m_name(name), m_address(address), m_returnType(returnType), m_gen(nullptr), m_script(nullptr)
	{}
	uint32_t GetAddress() const;
	std::string GetName() const;
	ScriptType GetReturnType() const;
	SymbolTable* GetLocalVars()
	{
		return &m_localVars;
	}
	
	void SetArguments(std::vector<ArgumentTypeInfo> args);
	void SetVariables(const CDataDeclList& declList);
	void SetGlobalSymTable(SymbolTable* symTable);
	void SetFrontend(CFrontend* script)
	{
		m_script = script;
	}
	void SetCfgFile(const std::string cfgFile)
	{
		m_cfgFile = cfgFile;
	}
	IRGenerator* GetIRGen() const
	{
		return m_gen;
	}
	friend std::ostream& operator<<(std::ostream& out, const CFunction& o);
	void Construct(const std::vector<AbstractExpression*>& expressions);
protected:
	void AddArgument(ArgumentTypeInfo typeInfo, size_t& nNums, size_t& nStrs, size_t& nObjs);
private:
	CFrontend* m_script; // TODO: Make a counterpart to installshield struct in decompiler and we won't need this
	IRGenerator* m_gen;
	SymbolTable m_localVars;
	SymbolTable m_argVars;
	ScriptType m_returnType;
	uint32_t m_address;
	std::string m_name;
	std::string m_cfgFile;
	size_t m_nArgNums = 0; 
	size_t m_nArgStrs = 0; 
	size_t m_nArgObjs = 0;
	size_t m_nLocalNums = 0;
	size_t m_nLocalStrs = 0;
};