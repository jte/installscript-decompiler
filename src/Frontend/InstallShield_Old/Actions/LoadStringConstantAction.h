#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

	class CLoadStringConstantAction : public CActionWithArgs
	{
	public:
		CLoadStringConstantAction(CIScript* script, StreamPtr& filePtr);
	protected:
		void print(std::ostream& os) const override;
		::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
		{
			std::vector<::AbstractExpression*> args;
			for (auto arg : m_arguments)
			{
				args.push_back(arg->ToExpression(symTable));
			}
			return new AssignExpression(new VariableExpression(symTable->GetByAddress(0, EVariableType::Number, true)), new FunctionCallExpression("LoadStringConstant", args));
		}
	private:
	};

};
