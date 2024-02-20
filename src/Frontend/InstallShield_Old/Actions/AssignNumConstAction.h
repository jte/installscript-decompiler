#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CAssignNumConstAction : public CActionWithArgs
{
public:
	CAssignNumConstAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
};

};
