#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

class CEndFuncAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override
	{
		return new NopExpression();
	}
public:
	CEndFuncAction(CIScript* script, StreamPtr& filePtr);
	//void Parse(StreamPtr& filePtr);
};