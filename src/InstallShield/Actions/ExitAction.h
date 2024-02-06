#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CExitAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
public:
	CExitAction(CIScript* script, StreamPtr& filePtr);
};