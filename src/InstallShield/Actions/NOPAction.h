#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CNOPAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
public:
	CNOPAction(CIScript* script, StreamPtr& filePtr);
};