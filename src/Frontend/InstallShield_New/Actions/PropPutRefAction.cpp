#include "PropPutRefAction.h"
#include <string>

namespace newis
{

void CPropPutRefAction::print(std::ostream& os) const
{
	os << "PutRef " << *(m_arguments[0]) << " " << *(m_arguments[1]);
}

};
