#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CDoHandlerAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new NopExpression();
	}
public:
	CDoHandlerAction(CIScript* script, StreamPtr& filePtr);
};

};
