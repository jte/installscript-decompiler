#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"
#include <sstream>

namespace oldis
{

class CPlaceholderAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		std::vector<::AbstractExpression*> args;
		for (auto arg : m_arguments)
		{
			args.push_back(arg->ToExpression(symTable));
		}
		std::stringstream ss;
		ss << "PlaceholderAction_0x" << std::hex << m_actionId;
		return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new FunctionCallExpression(ss.str(), args));
	}
public:
	CPlaceholderAction(CIScript* script, StreamPtr& filePtr);
};

};