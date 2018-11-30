#include "SetByteAction.h"
#include <cassert>
#include <string>
#include "ArrayAccessExpression.h"
#include "CNumConst.h"

void CSetByteAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 3);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CSetByteAction::ToStatement() const
{
	std::vector<CExpression*> exprs;
	CVariable* base = dynamic_cast<CVariable*>(m_arguments[0]->ToExpression());
	CNumConst* index = dynamic_cast<CNumConst*>(m_arguments[1]);
	CArrayAccessExpression* arr = new CArrayAccessExpression(base, index->GetConst(), CArrayAccessExpression::AccessType::StringArray);
	exprs.push_back(arr);
	exprs.push_back(m_arguments[2]->ToExpression());
	return std::make_shared<CStatement>(StatementType::Assign, exprs);
}