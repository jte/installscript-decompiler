#include "Variable.h"
#include "IArgument.h"
#include "Type.h"

void CVariable::print(std::ostream& out)
{
	if (m_name.empty())
		out << m_concreteType << m_address;
	else
		out << m_name;
}

bool CVariable::operator==(const CVariable& rhs)
{
	return m_concreteType == rhs.m_concreteType && m_address == rhs.m_address;
}

bool CVariable::IsLocal() const
{
	return m_address < 0;
}

bool CVariable::IsGlobal() const
{
	return !IsLocal();
}

CVariable::CVariable(ArgType type, int32_t address) :
	m_concreteType(type), m_address(address)
{
	if (m_address >= 0) // global
		m_name += "gbl";
	else
		m_name += "lcl";
	switch (m_concreteType)
	{
	case ArgType::NumArg:
		break;
	case ArgType::StrArg:
		break;
	case ArgType::VariantArg:
		break;
	}
	m_name += std::to_string(m_address < 0 ? -m_address : m_address);
}

int32_t CVariable::GetAddress() const
{
	return m_address;
}

ArgType CVariable::GetConcreteType() const
{
	return m_concreteType;
}

void CVariable::SetName(const std::string& name)
{
	m_name = name;
}

const std::string CVariable::GetName() const
{
	return m_name;
}