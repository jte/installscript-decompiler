#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Parser/Expressions.h"

namespace oldis
{

class CNopAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
public:
	CNopAction(CIScript* script, StreamPtr& filePtr);
};

};