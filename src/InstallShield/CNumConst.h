#pragma once

#include "StreamPtr.h"
#include "IArgument.h"
#include "Parser/Expressions.h"

class CNumConst : public IArgument
{
private:
	uint32_t m_num;
protected:
	void print(std::ostream& os) const;
	AbstractExpression* ToExpression() const override
	{
		return new NumberExpression(m_num);
	}
public:
	CNumConst(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	uint32_t GetConst() const;
};