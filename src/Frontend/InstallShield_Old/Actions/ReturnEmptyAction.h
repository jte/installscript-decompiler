#pragma once

#include "Action.h"
#include "StreamPtr.h"

namespace oldis
{

class CReturnEmptyAction : public CAction
{
public:
	CReturnEmptyAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
};

};
