#include "PropPutRefAction.h"
#include <string>

void CPropPutRefAction::print(std::ostream& os) const
{
	os << "PutRef " << *(m_arguments[0]) << " " << *(m_arguments[1]) << " " << *(m_arguments[2]);
}

CStatement CPropPutRefAction::ToStatement() const
{
	return CStatement(StatementType::PropPutRef, CExpression::FromScript(m_arguments));
}