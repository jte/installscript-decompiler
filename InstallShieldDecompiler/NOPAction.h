#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "Statement.h"

class CNOPAction : public CAction
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CNOPAction(CIScript* script, StreamPtr& filePtr);
};