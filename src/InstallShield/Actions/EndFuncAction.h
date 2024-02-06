#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CEndFuncAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
public:
	CEndFuncAction(CIScript* script, StreamPtr& filePtr);
	//void Parse(StreamPtr& filePtr);
};