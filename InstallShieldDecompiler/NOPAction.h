#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Statement.h"

class CNOPAction : public CAction
{
protected:
	void print(std::ostream& os) const;
	CStatement ToStatement() const;
public:
	CNOPAction(CIScript* script, StreamPtr& filePtr);
};