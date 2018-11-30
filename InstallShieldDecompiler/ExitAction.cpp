#include "ExitAction.h"
#include <string>

CExitAction::CExitAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CExitAction::print(std::ostream& os) const
{
	os << "EXIT";
}

std::shared_ptr<CStatement> CExitAction::ToStatement() const
{
	return std::make_shared<CStatement>(StatementType::Exit);
}