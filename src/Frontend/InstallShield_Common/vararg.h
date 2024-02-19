#pragma once

#include "IArgument.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"
#include <sstream>
#include "Variables/Variable.h"

template <ArgType type, bool parameter>
class vararg : public IArgument
{
protected:
	int16_t m_address;
	void print(std::ostream& os) const override
	{
		os << (parameter ? "p" : "v") << ' ' << type << ' ' << m_address;
	}

public:
	AbstractExpression* ToExpression(SymbolTable* symTable) const override
	{
		EVariableType varType;
		switch (type)
		{
		case ArgType::StrArg:
			varType = EVariableType::String;
			break;
		case ArgType::NumArg:
			varType = EVariableType::Number;
			break;
		case ArgType::VariantArg:
			varType = EVariableType::Variant;
			break;
		}
		return new VariableExpression(symTable->GetByAddress(m_address, varType, m_address > -100));
	}
	vararg(StreamPtr& filePtr)
	{
		Parse(filePtr);
		m_type = type;
	}
	void Parse(StreamPtr& filePtr) override
	{
		filePtr.Read(m_address);
	}
	int16_t GetAddress() const
	{
		return m_address;
	}
};