#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Parser/Expressions.h"

template <BinaryExprType Sym>
class CBinaryAction : public CActionWithArgs
{
public:
	CBinaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
protected:
	void print(std::ostream& os) const override
	{
		assert(m_arguments.size() == 3);
		os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << (int)Sym << ' ' << *(m_arguments[2]);
	}

	AbstractExpression* ToExpression(SymbolTable* symTable) const override {
		return new AssignExpression(m_arguments[0]->ToExpression(symTable), new BinaryExpression((char)Sym, m_arguments[1]->ToExpression(symTable), m_arguments[2]->ToExpression(symTable)));
	}
};