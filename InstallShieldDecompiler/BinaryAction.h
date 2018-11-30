#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Statement.h"
#include "BinaryExpression.h"

template <BinaryExprType Sym>
class CBinaryAction : public CActionWithArgs
{
public:
	CBinaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
protected:
	void print(std::ostream& os) const
	{
		assert(m_arguments.size() == 3);
		os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << (int)Sym << ' ' << *(m_arguments[2]);
	}
	std::shared_ptr<CStatement> ToStatement() const
	{
		std::vector<CExpression*> exprs;
		exprs.push_back(m_arguments[0]->ToExpression()); // destination
		CBinaryExpression* binExpr = new CBinaryExpression(Sym, m_arguments[1]->ToExpression(), m_arguments[2]->ToExpression());
		exprs.push_back(binExpr);
		return std::make_shared<CStatement>(StatementType::Assign, exprs);
	}
};