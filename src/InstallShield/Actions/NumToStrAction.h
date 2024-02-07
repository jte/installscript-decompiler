#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CNumToStrAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CNumToStrAction(CIScript* script, StreamPtr& filePtr);
};