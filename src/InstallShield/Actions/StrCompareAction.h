#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrCompareAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CStrCompareAction(CIScript* script, StreamPtr& filePtr);
};