#pragma once

#include "StreamPtr.h"
#include "Type.h"
#include "Variables/SymbolTable.h"

class AbstractExpression;

class IArgument
{
protected:
	ArgType m_type;
	virtual void print(std::ostream& os) const = 0;
public:
	virtual void Parse(StreamPtr& filePtr) = 0;
	friend std::ostream& operator<<(std::ostream& out, const IArgument& o);
	virtual AbstractExpression* ToExpression(SymbolTable* symTable) const = 0;
};