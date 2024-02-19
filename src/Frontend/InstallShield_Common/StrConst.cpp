#include "StrConst.h"

CStrConst::CStrConst(StreamPtr& filePtr)
{
	Parse(filePtr);
	m_type = ArgType::StrArg;
}

void CStrConst::Parse(StreamPtr& filePtr)
{
	uint16_t length = 0;
	filePtr.Read(length);
	std::vector<char> string;
	filePtr.ReadArray(string, length);
	m_string = std::string(string.begin(), string.end());
}

const std::string CStrConst::GetString() const
{
	std::string str;
	for (auto elem : m_string)
	{
		if (elem == '\n')
		{
			str += "\\n";
		}
		else if (elem == '\r')
		{
			str += "\\r";
		}
		else if (elem == '\t')
		{
			str += "\\t";
		}
		else
		{
			str += elem;
		}
	}
	return str;
}

void CStrConst::print(std::ostream& os) const
{
	os << '"' << GetString() << '"';
}