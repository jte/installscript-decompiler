#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CAddressOfWideAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override {
		return new AssignExpression(m_arguments[0]->ToExpression(symTable), new AddressOfExpression(m_arguments[1]->ToExpression(symTable)));
	}
public:
	CAddressOfWideAction(CIScript* script, StreamPtr& filePtr);
};