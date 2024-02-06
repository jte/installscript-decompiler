#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CAssignAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override {
		return new AssignExpression(m_arguments[0]->ToExpression(), m_arguments[1]->ToExpression());
	}
public:
	CAssignAction(CIScript* script, StreamPtr& filePtr);
};