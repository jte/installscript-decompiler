#include "EndCatchAction.h"
#include <cassert>
#include <string>

void CEndCatchAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}