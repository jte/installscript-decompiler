#include "AddressOfWideAction.h"
#include <cassert>
#include <string>

void CAddressOfWideAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CAddressOfWideAction::CAddressOfWideAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}