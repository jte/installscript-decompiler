#pragma once

#include "StreamPtr.h"
#include "IArgument.h"
#include <string>
#include "Parser/Expressions.h"

class CStrConst : public IArgument
{
private:
	std::string m_string;
protected:
	void print(std::ostream& os) const override;
	AbstractExpression* ToExpression() const override
	{
		return new StringExpression(m_string);
	}
public:
	CStrConst(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr) override;
	const std::string GetString() const;
};