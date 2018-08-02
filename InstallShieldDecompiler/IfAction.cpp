#include "IfAction.h"
#include <cassert>
#include <string>
#include "Variable.h"

void CIfAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << "If" << *(m_arguments[0]) << " " << *(m_arguments[1]);
}

CStatement CIfAction::ToStatement() const
{
	return CStatement(StatementType::If, CVariable::FromScript(m_arguments));
}