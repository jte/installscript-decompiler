#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CPropPutAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CPropPutAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};