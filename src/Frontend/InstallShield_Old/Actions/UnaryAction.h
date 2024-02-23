#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace oldis
{

	template <int Sym>
	class CUnaryAction : public CActionWithArgs
	{
	public:
		CUnaryAction(CIScript* script, StreamPtr& filePtr) :
			CActionWithArgs(script, filePtr)
		{
			m_numOperands = 3;
			m_arguments.resize(m_numOperands);
			ParseArguments(filePtr);
		}
	protected:
		void print(std::ostream& os) const override
		{
			os << *(m_arguments[0]) << " = " << (char)Sym << *(m_arguments[1]);
		}
		::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
		{
			return new AssignExpression(m_arguments[0]->ToExpression(symTable), new UnaryExpression((char)Sym, m_arguments[1]->ToExpression(symTable)));
		}
	};

};
