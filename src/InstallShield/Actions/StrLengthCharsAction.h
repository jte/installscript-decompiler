#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrLengthCharsAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CStrLengthCharsAction(CIScript* script, StreamPtr& filePtr);
};