#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

class CNumToStrAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override
	{
		std::vector<AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression(symTable));
		args.push_back(m_arguments[1]->ToExpression(symTable));
		return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new FunctionCallExpression("NumToStr", args));
	}
public:
	CNumToStrAction(CIScript* script, StreamPtr& filePtr);
};