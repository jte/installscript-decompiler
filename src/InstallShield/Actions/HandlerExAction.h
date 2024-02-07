#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CHandlerExAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CHandlerExAction(CIScript* script, StreamPtr& filePtr);
};