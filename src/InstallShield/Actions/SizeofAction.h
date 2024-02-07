#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CSizeofAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CSizeofAction(CIScript* script, StreamPtr& filePtr);
};