#pragma once

#include "FuncCallAction.h"
#include "Parser/Expressions.h"

namespace newis
{

class CDLLFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override;
public:
	CDLLFuncCallAction(CIScript* script, StreamPtr& filePtr);
};

};