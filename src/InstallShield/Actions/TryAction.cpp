#include "TryAction.h"
#include <cassert>
#include <string>

void CTryAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << "TryAction [0]:" << *(m_arguments[0]) << " [1]: " << *(m_arguments[1]);
}