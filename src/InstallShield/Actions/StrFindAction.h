#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CStrFindAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CStrFindAction(CIScript* script, StreamPtr& filePtr);
};