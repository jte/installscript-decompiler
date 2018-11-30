#pragma once

#include "ActionWithArgs.h"

class CAddressOfAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CAddressOfAction(CIScript* script, StreamPtr& filePtr);
};