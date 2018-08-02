#include "AbortAction.h"
#include <string>

CAbortAction::CAbortAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CAbortAction::print(std::ostream& os) const
{
	os << "NOP";
}

CStatement CAbortAction::ToStatement() const
{
	return CStatement(StatementType::Abort);
}