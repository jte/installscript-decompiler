#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

	class CReturnAction : public CActionWithArgs
	{
	public:
		CReturnAction(CIScript* script, StreamPtr& filePtr);
	protected:
		void print(std::ostream& os) const override;
		::AbstractExpression* ToExpression(::SymbolTable* symTable) const override {
			return new ReturnExpression(m_arguments.size() != 0 ? m_arguments[0]->ToExpression(symTable) : nullptr);
		}
	private:
	};

};
