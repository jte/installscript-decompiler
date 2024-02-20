#pragma once

#include "ActionWithArgs.h"

namespace oldis
{

class CInternalFuncCallAction : public CActionWithArgs
{
protected:
	uint8_t m_protoCode;
	uint16_t m_PrototypeIndex;
	uint8_t m_labelCode;
	uint16_t m_eventIndex;

	void print(std::ostream& os) const override;
public:
	CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr);
};

};