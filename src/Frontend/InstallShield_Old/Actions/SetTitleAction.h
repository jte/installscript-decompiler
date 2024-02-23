#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace oldis
{

class CSetTitleAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		std::vector<::AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression(symTable));
		return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new FunctionCallExpression("SetTitle", args));
	}
public:
	CSetTitleAction(CIScript* script, StreamPtr& filePtr);
};

};