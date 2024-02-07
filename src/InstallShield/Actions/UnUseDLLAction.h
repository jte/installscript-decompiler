#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CUnUseDLLAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CUnUseDLLAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};