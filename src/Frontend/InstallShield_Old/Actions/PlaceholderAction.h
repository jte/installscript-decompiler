#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

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
			return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new FunctionCallExpression("PlaceholderAction", args));
		}
	public:
		CPlaceholderAction(CIScript* script, StreamPtr& filePtr);
	};

};