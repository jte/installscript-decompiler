#pragma once

#include <vector>

class CExpression
{
private:
public:
	virtual ~CExpression()
	{}
	static std::vector<CExpression*> FromScript(std::vector<class IArgument*> args);
	virtual void print(std::ostream& out) = 0;
	friend std::ostream& operator<<(std::ostream& out, CExpression* e);
private:
};