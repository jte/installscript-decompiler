#include "AssignAction.h"
#include "Variable.h"
#include <cassert>
#include <string>

void CAssignAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

CStatement CAssignAction::ToStatement() const
{
	return CStatement(StatementType::Assign, CVariable::FromScript(m_arguments));
}

CAssignAction::CAssignAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{}