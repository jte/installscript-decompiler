#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CGotoAction : public CActionWithArgs
{
public:
	CGotoAction(CIScript* script, StreamPtr& filePtr);
	uint16_t GetTarget() const
	{
		return m_bbIndex;
	}
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new GotoExpression(nullptr);
	}
private:
	uint16_t m_bbIndex;//event index
};

};
