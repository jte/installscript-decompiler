#include "AddressOfWideAction.h"
#include <cassert>
#include <string>

void CAddressOfWideAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CStatement CAddressOfWideAction::ToStatement() const
{
	return CStatement(StatementType::AddressOfWide, CExpression::FromScript(m_arguments));
}

CAddressOfWideAction::CAddressOfWideAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}