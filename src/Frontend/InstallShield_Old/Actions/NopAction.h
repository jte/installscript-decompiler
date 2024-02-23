#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace oldis
{

class CNopAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
		return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)));
	}
public:
	CNopAction(CIScript* script, StreamPtr& filePtr);
};

};