#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CHandlerExAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new NopExpression();
	}
public:
	CHandlerExAction(CIScript* script, StreamPtr& filePtr);
};

};
