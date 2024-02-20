#pragma once

#include "ActionWithArgs.h"

namespace oldis
{

class CIfAction : public CActionWithArgs
{
public:
	CIfAction(CIScript* script, StreamPtr& filePtr);
protected:
	void print(std::ostream& os) const override;
private:
	uint8_t m_label;
	uint16_t m_realAddress;
};

};