#pragma once

#include <ostream>

class SymbolTable;
class AbstractExpression;

class CAction
{
public:
	friend std::ostream& operator<<(std::ostream& out, const CAction& o);
	virtual AbstractExpression* ToExpression(SymbolTable* symTable) const { return nullptr; }
protected:
	virtual void print(std::ostream& os) const = 0;
private:
};
