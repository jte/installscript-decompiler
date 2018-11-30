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

std::shared_ptr<CStatement> CAbortAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::Abort);
}