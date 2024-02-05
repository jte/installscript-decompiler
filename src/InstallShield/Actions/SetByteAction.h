#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CSetByteAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;

	AbstractExpression* ToExpression() const override {
		auto lhs = new ArrayAccessExpression(dynamic_cast<VariableExpression*>(m_arguments[0]->ToExpression()), m_arguments[1]->ToExpression());
		return new AssignExpression(lhs, m_arguments[2]->ToExpression());
	}
public:
	CSetByteAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};