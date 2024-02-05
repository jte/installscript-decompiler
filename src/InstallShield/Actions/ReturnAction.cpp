#include "ReturnAction.h"
#include <string>
#include <cassert>
#include "vararg.h"

void CReturnAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 0 || m_arguments.size() == 1);
	os << "RETURN "; 
	if (m_arguments.size() == 1)
		os << *(m_arguments[0]);
}