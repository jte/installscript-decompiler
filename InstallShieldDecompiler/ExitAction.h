#pragma once

#include "Action.h"
#include "StreamPtr.h"

class CExitAction : public CAction
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CExitAction(CIScript* script, StreamPtr& filePtr);
};