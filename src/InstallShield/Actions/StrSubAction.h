#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrSubAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CStrSubAction(CIScript* script, StreamPtr& filePtr);
};