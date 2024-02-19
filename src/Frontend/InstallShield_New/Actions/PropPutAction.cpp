#include "PropPutAction.h"
#include <string>
#include "StrConst.h"

namespace newis
{

void CPropPutAction::print(std::ostream& os) const
{
	os << "Put (arg size: " << m_arguments.size() << ") ";
	for (auto arg : m_arguments)
	{
		os << *arg << " ";
	}
}

};
