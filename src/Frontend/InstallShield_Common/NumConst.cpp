#include "NumConst.h"
#include <string>

CNumConst::CNumConst(StreamPtr& filePtr)
{
	Parse(filePtr);
	m_type = ArgType::NumArg;
}

void CNumConst::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_num);
}

int32_t CNumConst::GetConst() const
{
	return m_num;
}

void CNumConst::print(std::ostream& os) const
{
	os << GetConst();
}