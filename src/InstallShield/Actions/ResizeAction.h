#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CResizeAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CResizeAction(CIScript* script, StreamPtr& filePtr);
};