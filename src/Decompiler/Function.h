#pragma once

#include "CFG/ControlFlowGraph.h"
#include "CFG/BasicBlock.h"
#include "ISBasicBlock.h"
#include "IRGen/IRStatements.h"
#include "GraphVizPrinter.h"
#include <vector>
#include <string>
#include <set>
#include <stdexcept>
#include <iostream>
#include <stack>
#include "Parser/Expressions.h"
#include "../InstallShield/Type.h"
#include "DataDeclList.h"

class IRGenerator;

class CFunction
{
public:
	CFunction(int32_t address, ScriptType returnType) :
		m_address(address), m_returnType(returnType), m_gen(nullptr)
	{}
	CFunction(std::string name, int32_t address, ScriptType returnType) :
		m_name(name), m_address(address), m_returnType(returnType), m_gen(nullptr)
	{}
	uint32_t GetAddress() const;
	std::string GetName() const;
	ScriptType GetReturnType() const;
	void SetArguments(std::vector<ArgumentTypeInfo> args);
	void SetVariables(const CDataDeclList& declList);

	friend std::ostream& operator<<(std::ostream& out, const CFunction& o);
	void Construct(const std::vector<AbstractExpression*>& expressions);
protected:
	void AddArgument(ArgumentTypeInfo typeInfo, size_t& nNums, size_t& nStrs, size_t& nObjs);
private:
	IRGenerator* m_gen;
	ScriptType m_returnType;
	uint32_t m_address;
	std::string m_name;
	size_t m_nArgNums = 0; 
	size_t m_nArgStrs = 0; 
	size_t m_nArgObjs = 0;
	size_t m_nLocalNums = 0;
	size_t m_nLocalStrs = 0;
	CDataDeclList m_declList;
	std::vector<VariableExpression*> m_arguments;
};