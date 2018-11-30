#pragma once

#include <cstdint>
#include <string>
#include "Type.h"
#include <vector>
#include "Expression.h"

class CVariable : public CExpression
{
private:
	int32_t m_address;
	std::string m_name;
	ArgType m_concreteType; // always present
	ScriptType m_scriptType; // must be inferred for local variables
	bool m_isArgument;
public:
	bool operator==(const CVariable& rhs);
	CVariable(ArgType type, int32_t address, bool isArgument);
	int32_t GetAddress() const;
	ArgType GetConcreteType() const;
	void SetName(const std::string& name);
	const std::string GetName() const;
	void print(std::ostream& out);
	bool IsLocal() const;
	bool IsGlobal() const;
};