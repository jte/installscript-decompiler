#pragma once

#include "Statement.h"

class CIfStatement : public CStatement
{
public:
private:
	CBasicBlock* trueBranch;
	CBasicBlock* falseBranch;
};