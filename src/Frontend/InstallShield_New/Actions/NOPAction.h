#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace newis
{

class CNOPAction : public CAction
{
protected:
	void print(std::ostream& os) const override;

	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
		return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)));
	}
public:
	CNOPAction(CIScript* script, StreamPtr& filePtr);
};

};