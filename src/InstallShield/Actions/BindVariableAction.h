#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CBindVariableAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	AbstractExpression* ToExpression() const override {
		return new AssignExpression(m_arguments[0]->ToExpression(), m_arguments[1]->ToExpression());
	}
public:
	CBindVariableAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};