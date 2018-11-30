#include "IfAction.h"
#include <cassert>
#include <string>
#include "Variable.h"
#include "BinaryExpression.h"
#include "Literal.h"

void CIfAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << "If" << *(m_arguments[0]) << " " << *(m_arguments[1]);
}

std::shared_ptr<CStatement> CIfAction::ToStatement() const
{
	//m_arguments[0] -> if level
	std::vector<CExpression*> exprs;
	exprs.push_back(m_arguments[1]->ToExpression());//expression
	exprs.push_back(m_arguments[0]->ToExpression());//else branch relative bb addr
	return std::make_shared<CStatement>(StatementType::If, exprs);
}