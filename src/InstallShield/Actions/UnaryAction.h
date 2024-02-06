#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Parser/Expressions.h"

template <int Sym>
class CUnaryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override
	{
		assert(m_arguments.size() == 3);
		os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << Sym << ' ' << *(m_arguments[2]);
	}
	AbstractExpression* ToExpression() const override 
	{
		return new AssignExpression(m_arguments[0]->ToExpression(), new UnaryExpression((char)Sym, m_arguments[1]->ToExpression()));
	}
public:
	CUnaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};