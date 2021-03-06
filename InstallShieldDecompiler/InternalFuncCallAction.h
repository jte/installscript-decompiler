#pragma once

#include "FuncCallAction.h"

class CInternalFuncCallAction : public CFuncCallAction
{
protected:
	void print(std::ostream& os) const;
	std::shared_ptr<CStatement> ToStatement() const;
public:
	CInternalFuncCallAction(CIScript* script, StreamPtr& filePtr);
	IArgument* ParseArgument(StreamPtr& filePtr);
};