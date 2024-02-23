#pragma once

#include "Action.h"
#include "StreamPtr.h"

namespace oldis
{

class CReturnEmptyAction : public CAction
{
public:
	CReturnEmptyAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
		return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)));
	}
private:
};

};
