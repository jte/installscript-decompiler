#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace newis
{

class CStrLengthCharsAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		std::vector<::AbstractExpression*> args;
		args.push_back(m_arguments[0]->ToExpression(symTable));
		return new AssignExpression(new VariableExpression(symTable->GetByName("LAST_RESULT", EVariableType::Variant, true)), new FunctionCallExpression("StrLengthChars ", args));
	}
public:
	CStrLengthCharsAction(CIScript* script, StreamPtr& filePtr);
};

};