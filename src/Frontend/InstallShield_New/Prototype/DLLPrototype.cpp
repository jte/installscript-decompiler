#include "DLLPrototype.h"

namespace newis
{

CDLLPrototype::CDLLPrototype(StreamPtr& filePtr) :
	CPrototype(filePtr)
{
	Parse(filePtr);
}

void CDLLPrototype::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_returnType);
	m_dllName = filePtr.ReadInsString();
	m_name = filePtr.ReadInsString();
	filePtr.Read(m_bbId);
	ParseArguments(filePtr);
}

void CDLLPrototype::print(std::ostream& os) const
{
	os << "prototype ";
	if (m_flags & (uint8_t)CPrototype::Flags::HasStdcallConvention)
	{
		os << "stdcall";
	}
	else
	{
		os << "cdecl";
	}
	os << " " << m_returnType << " ";
	os << m_dllName << "." << m_name;
	os << "(";
	os << m_arguments;
	os << ")";
	os << " // @ " << GetBBId();
}

std::string CDLLPrototype::GetDLLName() const
{
	return m_dllName;
}

};