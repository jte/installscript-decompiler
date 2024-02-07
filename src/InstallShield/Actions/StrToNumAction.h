#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrToNumAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CStrToNumAction(CIScript* script, StreamPtr& filePtr);
};