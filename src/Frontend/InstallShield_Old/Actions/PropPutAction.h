#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CPropPutAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
public:
	CPropPutAction(CIScript* script, StreamPtr& filePtr);
};

};