#include "BindVariableAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CBindVariableAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

};
