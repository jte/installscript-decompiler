#include "InternalPrototype.h"

CInternalPrototype::CInternalPrototype(StreamPtr& filePtr) :
	CPrototype(filePtr)
{
	Parse(filePtr);
}

void CInternalPrototype::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_returnType);
	uint16_t align;
	filePtr.Read(align);
	m_name = filePtr.ReadInsString();
	filePtr.Read(m_bbId);
	ParseArguments(filePtr);
}

void CInternalPrototype::print(std::ostream& os) const
{
	if (m_flags & (uint8_t)Flags::IsExported)
	{ 
		os << "export ";
	}
	os << "prototype ";
	os << m_returnType << ' ';
	if ((m_flags & (uint8_t)Flags::IsExported) || !m_name.empty())
	{
		os << m_name;
	}
	//else
	{
		os << "(addr:" << GetBBId() << ")";
	}
	os << '(';
	os << m_arguments;
	os << ");";
}