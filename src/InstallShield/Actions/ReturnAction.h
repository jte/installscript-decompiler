#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CReturnAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override {
		return new ReturnExpression(m_arguments.size() != 0 ? m_arguments[0]->ToExpression() : nullptr);
	}
public:
	CReturnAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};