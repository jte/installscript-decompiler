#pragma once

#include "Action.h"
#include "StreamPtr.h"

namespace oldis
{

class CReturnFromProgramAction : public CAction
{
public:
	CReturnFromProgramAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
};

};
