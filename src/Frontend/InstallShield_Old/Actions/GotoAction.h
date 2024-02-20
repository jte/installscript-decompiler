#pragma once

#include "ActionWithArgs.h"
#include "StreamPtr.h"

namespace oldis
{

class CGotoAction : public CActionWithArgs
{
public:
	CGotoAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
	uint16_t m_bbIndex;//event index
};

};
