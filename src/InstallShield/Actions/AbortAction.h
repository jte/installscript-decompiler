#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CAbortAction : public CAction
{
protected:
	void print(std::ostream& os) const;
public:
	CAbortAction(CIScript* script, StreamPtr& filePtr);
};