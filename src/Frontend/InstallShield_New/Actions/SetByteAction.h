#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CSetByteAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;

	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
		auto lhs = new ArrayAccessExpression(dynamic_cast<VariableExpression*>(m_arguments[0]->ToExpression(symTable)), m_arguments[1]->ToExpression(symTable));
		return new AssignExpression(lhs, m_arguments[2]->ToExpression(symTable));
	}
public:
	CSetByteAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};

};