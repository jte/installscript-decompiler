#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CEndCatchAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CEndCatchAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};