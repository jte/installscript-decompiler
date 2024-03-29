#include "AssignAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CAssignAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CAssignAction::CAssignAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}

};
