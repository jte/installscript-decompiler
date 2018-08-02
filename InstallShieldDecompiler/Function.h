#pragma once

#include "Variable.h"
#include "BasicBlock.h"
#include <vector>
#include <string>
#include <set>

class CFunction
{
private:
	std::vector<CVariable*> m_numbers;
	std::vector<CVariable*> m_objects;
	std::vector<CVariable*> m_strings;
	std::vector<CBasicBlock*> m_bbs;
	ScriptType m_returnType;
	uint32_t m_address;
	std::string m_name;
	std::vector<ArgumentTypeInfo> m_arguments;
	void AddArgument(ArgumentTypeInfo typeInfo);
	void GatherVariables(const CBasicBlock& bb);
	void AddVariable(CVariable* var);
public:
	CFunction(int32_t address, ScriptType returnType) :
		m_address(address), m_returnType(returnType)
	{}
	CFunction(std::string name, int32_t address, ScriptType returnType) :
		m_name(name), m_address(address), m_returnType(returnType)
	{}
	uint32_t GetAddress() const;
	std::string GetName() const;
	ScriptType GetReturnType() const;
	void AddBasicBlock(CBasicBlock* bb);
	void SetArguments(std::vector<ArgumentTypeInfo> args);

	friend std::ostream& operator<<(std::ostream& out, const CFunction& o);
};