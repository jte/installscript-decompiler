#include "Prototype.h"
#include "InternalPrototype.h"
#include "DLLPrototype.h"
#include "Type.h"

CPrototype::CPrototype(StreamPtr& filePtr) :
	m_bbId(-1)
{
	filePtr.Read(m_flags);
}

void CPrototype::ParseArguments(StreamPtr& filePtr)
{
	uint16_t numArguments = 0;
	filePtr.Read(numArguments);
	m_arguments.resize(numArguments);

	for (auto& arg : m_arguments)
	{
		filePtr.Read(arg.scriptType);
		filePtr.Read(arg.internalType);
	}
}

static struct
{
	size_t match;
	CPrototype::MakeFn fn;
} factories[] =
{
	1, CDLLPrototype::Make,
	2, CInternalPrototype::Make,
};

CPrototype::MakeFn CPrototype::FindFactory(size_t flags)
{
	for (auto& f : factories)
	{
		if (flags & f.match)
			return f.fn;
	}
	throw std::runtime_error("Unknown prototype found");
}

std::string CPrototype::GetName() const
{
	return m_name;
}

CPrototype::Flags CPrototype::GetFlags() const
{
	return (CPrototype::Flags)m_flags;
}

ScriptType CPrototype::GetReturnType() const
{
	return m_returnType;
}

std::ostream& operator<<(std::ostream& out, const CPrototype& o)
{
	o.print(out);
	out << std::endl;
	return out;
}

void CPrototype::print(std::ostream& os) const
{
	if (m_flags & (uint8_t)Flags::IsExported)
	{
		os << "export ";
	}
	os << "prototype ";
	os << m_returnType << ' ';
	if (m_flags & (uint8_t)Flags::IsExported)
	{
		os << m_name;
	}
	else
	{
		os << "predefined_func";
	}
	os << '(';
	os << m_arguments;
	os << ");";
}

bool CPrototype::GetIsExported() const
{
	return m_flags & (uint8_t)Flags::IsExported;
}

bool CPrototype::GetHasStdcallConvention() const
{
	return m_flags & (uint8_t)Flags::HasStdcallConvention;
}

bool CPrototype::GetIsGetProperty() const
{
	return m_flags & (uint8_t)Flags::IsGetProperty;
}

bool CPrototype::GetIsSetProperty() const
{
	return m_flags & (uint8_t)Flags::IsSetProperty;
}

bool CPrototype::GetHasVariableArgs() const
{
	return m_flags & (uint8_t)Flags::HasVariableArgs;
}

std::vector<ArgumentTypeInfo> CPrototype::GetArgumentTypeInfo() const
{
	return m_arguments;
}

std::ostream& operator<<(std::ostream& out, const std::vector<ArgumentTypeInfo>& arguments)
{
	for (auto it = arguments.begin(); it != arguments.end(); ++it)
	{
		out << it->scriptType;
		if (it != arguments.end() - 1)
			out << ',';
	}
	return out;
}