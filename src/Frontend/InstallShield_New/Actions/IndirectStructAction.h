#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Parser/Expressions.h"
#include "Variables/SymbolTable.h"

namespace newis
{

class CIndirectStructAction : public CActionWithArgs
{
protected:
	uint16_t m_structId = 0;
	void print(std::ostream& os) const override
	{
	}
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new UnaryExpression('*', m_arguments[0]->ToExpression(symTable)));
	}
public:
	CIndirectStructAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script)
	{
		uint16_t actionId = 0;
		filePtr.Read(actionId);
		filePtr.Read(m_structId);

		m_numOperands = 1;

		m_arguments.resize(1);
		ParseArguments(filePtr);
	}
};

};
