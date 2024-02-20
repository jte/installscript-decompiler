#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CAssignStringConstAction : public CActionWithArgs
{
public:
	CAssignStringConstAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
};

};
