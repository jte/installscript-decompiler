#include "GetByteAction.h"
#include <cassert>
#include <string>
#include "CNumConst.h"

void CGetByteAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 3);
	os << "GET BYTE " << * (m_arguments[0]) << " = " << *(m_arguments[1]) << "( " << *(m_arguments[2]) << ") ";
}