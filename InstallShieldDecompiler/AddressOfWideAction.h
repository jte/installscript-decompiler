#pragma once

#include "ActionWithArgs.h"

class CAddressOfWideAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CAddressOfWideAction(CIScript* script, StreamPtr& filePtr);
};