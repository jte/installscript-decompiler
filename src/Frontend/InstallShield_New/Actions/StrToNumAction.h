#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CStrToNumAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		std::vector<::AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression(symTable));
		args.push_back(m_arguments[1]->ToExpression(symTable));
		return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new FunctionCallExpression("StrToNum", args));
	}
public:
	CStrToNumAction(CIScript* script, StreamPtr& filePtr);
};

};