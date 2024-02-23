#pragma once

#include "ActionWithArgs.h"

namespace oldis
{

class CIfAction : public CActionWithArgs
{
public:
	CIfAction(CIScript* script, StreamPtr& filePtr);
	uint16_t GetContinueBranch() const
	{
		return m_realAddress;
	}
protected:
	void print(std::ostream& os) const override;
	::AbstractExpression* ToExpression(::SymbolTable* symTable) const override
	{
		return new IfExpression(m_arguments[0]->ToExpression(symTable), nullptr, nullptr);
	}
private:
	uint8_t m_label;
	uint16_t m_realAddress;
};

};