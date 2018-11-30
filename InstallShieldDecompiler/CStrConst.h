#pragma once

#include "StreamPtr.h"
#include "IArgument.h"
#include <string>
#include "Literal.h"

class CStrConst : public IArgument
{
private:
	std::string m_string;
protected:
	void print(std::ostream& os) const;
	CExpression* ToExpression() const
	{
		return new CStringLiteral(m_string);
	}
public:
	CStrConst(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	const std::string GetString() const;
};