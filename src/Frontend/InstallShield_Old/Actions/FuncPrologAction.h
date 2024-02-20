#pragma once

#include "Action.h"
#include "StreamPtr.h"

namespace oldis
{

class CFuncPrologAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	uint16_t m_numNumbers;
	std::vector<uint16_t> m_stringTable;
public:
	CFuncPrologAction(CIScript* script, StreamPtr& filePtr);
};

};
