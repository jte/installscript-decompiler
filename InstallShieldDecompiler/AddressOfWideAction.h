#pragma once

#include "ActionWithArgs.h"

class CAddressOfWideAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CAddressOfWideAction(CIScript* script, StreamPtr& filePtr);
};