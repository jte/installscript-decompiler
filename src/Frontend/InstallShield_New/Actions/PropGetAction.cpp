#include "PropGetAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CPropGetAction::print(std::ostream& os) const
{
	os << "Get " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

};