#include "GetByteAction.h"
#include <cassert>
#include <string>
#include "CNumConst.h"
#include "ArrayAccessExpression.h"

void CGetByteAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 3);
	os << *(m_arguments[0]) << " = " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CGetByteAction::ToStatement() const
{
	std::vector<CExpression*> exprs;
	CVariable* dest = dynamic_cast<CVariable*>(m_arguments[0]->ToExpression());
	CVariable* src = dynamic_cast<CVariable*>(m_arguments[1]->ToExpression());

	CNumConst* index = dynamic_cast<CNumConst*>(m_arguments[2]);
	CArrayAccessExpression* arr = new CArrayAccessExpression(src, index->GetConst(), CArrayAccessExpression::AccessType::StringArray);
	exprs.push_back(dest);
	exprs.push_back(arr);
	return std::make_shared<CStatement>(StatementType::Assign, exprs);
}