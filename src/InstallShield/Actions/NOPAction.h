#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

class CNOPAction : public CAction
{
protected:
	void print(std::ostream& os) const override;

	AbstractExpression* ToExpression() const override
	{
		return new NopExpression();
	}
public:
	CNOPAction(CIScript* script, StreamPtr& filePtr);
};