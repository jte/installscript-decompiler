#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CGetByteAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override {
		auto rhs = new ArrayAccessExpression(dynamic_cast<VariableExpression*>(m_arguments[1]->ToExpression(symTable)), m_arguments[2]->ToExpression(symTable));
		return new AssignExpression(m_arguments[0]->ToExpression(symTable), rhs);
	}
public:
	CGetByteAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};