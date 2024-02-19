#pragma once

#include "Variable.h"

class CNumberVariable : public CVariable
{
public:
	CNumberVariable(int address) : CVariable(address, EVariableType::Number)
	{

	}
protected:
	void print(std::ostream& os) const override
	{
		os << "INT " << GetName();
	}
private:
};