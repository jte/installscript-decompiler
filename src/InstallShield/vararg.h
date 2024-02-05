#pragma once

#include "IArgument.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"
#include <sstream>

template <ArgType type, bool parameter>
class vararg : public IArgument
{
protected:
	int16_t m_address;
	void print(std::ostream& os) const
	{
		os << (parameter ? "p" : "v") << ' ' << type << ' ' << m_address;
	}

public:
	AbstractExpression* ToExpression() const override
	{
		std::stringstream ss;
		ss << ((m_address <= -100) ? "Lcl" : "Gbl");
		ss << (parameter ? "Param" : "Var");
		switch (type)
		{
		case ArgType::StrArg:
			ss << "Str";
			break;
		case ArgType::NumArg:
			ss << "Num";
			break;
		case ArgType::VariantArg:
			ss << "Obj";
			break;
		}
		ss << (m_address <= -100 ? (m_address+100)*-1 : m_address);
		return new VariableExpression(ss.str());

		//ss << (parameter ? "p" : "v") << ' ' << type << ' ' << m_address;
		//return new VariableExpression(ss.str());
	}
	vararg(StreamPtr& filePtr)
	{
		Parse(filePtr);
		m_type = type;
	}
	void Parse(StreamPtr& filePtr)
	{
		filePtr.Read(m_address);
		//if (static_cast<int16_t>(m_address) <= -100)
		//{
		//	m_address = static_cast<uint16_t>(m_address + 100);
		//}
	}
	int16_t GetAddress() const
	{
		return m_address;
	}
};