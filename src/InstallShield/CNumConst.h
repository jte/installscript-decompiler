#pragma once

#include "StreamPtr.h"
#include "IArgument.h"
#include "Parser/Expressions.h"

class CNumConst : public IArgument
{
private:
	int32_t m_num;
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression(SymbolTable* symTable) const override
	{
		return new NumberExpression(m_num);
	}
public:
	CNumConst(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr) override;
	int32_t GetConst() const;
};