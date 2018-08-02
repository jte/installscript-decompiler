#include "AddressOfAction.h"
#include <cassert>
#include <string>

void CAddressOfAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CStatement CAddressOfAction::ToStatement() const
{
	return CStatement(StatementType::AddressOf, CExpression::FromScript(m_arguments));
}

CAddressOfAction::CAddressOfAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}