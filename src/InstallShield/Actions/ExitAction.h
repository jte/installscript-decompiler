#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CExitAction : public CAction
{
protected:
	void print(std::ostream& os) const;
public:
	CExitAction(CIScript* script, StreamPtr& filePtr);
};