#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "CNumConst.h"

class CFuncPrologAction : public CAction
{
protected:
	void print(std::ostream& os) const;
	uint32_t m_tableOffset;
	CStatement ToStatement() const;
public:
	CFuncPrologAction(CIScript* script, StreamPtr& filePtr);
};