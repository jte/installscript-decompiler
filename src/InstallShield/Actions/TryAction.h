#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CTryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override
	{
		return new NopExpression();
	}
public:
	CTryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};