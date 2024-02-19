#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace newis
{

class CAbortAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new AbortExpression();
	}
public:
	CAbortAction(CIScript* script, StreamPtr& filePtr);
};

};
