#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

class CExitAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CExitAction(CIScript* script, StreamPtr& filePtr);
};