#pragma once

#include "FuncCallAction.h"
#include "Parser/Expressions.h"

namespace newis
{

class CInternalFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override;
public:
	CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr);
	IArgument* ParseArgument(StreamPtr& filePtr) override;
};

};