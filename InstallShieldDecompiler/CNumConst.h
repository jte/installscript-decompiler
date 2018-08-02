#pragma once

#include "StreamPtr.h"
#include "IArgument.h"
#include "Literal.h"

class CNumConst : public IArgument
{
private:
	uint32_t m_num;
protected:
	void print(std::ostream& os) const;
	CExpression* ToExpression() const
	{
		return new CNumberLiteral(m_num);
	}
public:
	CNumConst(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	uint32_t GetConst() const;
};