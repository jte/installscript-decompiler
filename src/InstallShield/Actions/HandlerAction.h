#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CHandlerAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CHandlerAction(CIScript* script, StreamPtr& filePtr);
};