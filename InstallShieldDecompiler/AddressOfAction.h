#pragma once

#include "ActionWithArgs.h"

class CAddressOfAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CAddressOfAction(CIScript* script, StreamPtr& filePtr);
};