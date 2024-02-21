#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CPropGetAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const override;
public:
	CPropGetAction(CIScript* script, StreamPtr& filePtr);
};

};