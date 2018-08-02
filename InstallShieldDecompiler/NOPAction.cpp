#include "NOPAction.h"
#include "Statement.h"
#include <string>

CNOPAction::CNOPAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
}

void CNOPAction::print(std::ostream& os) const
{
	os << "NOP";
}

CStatement CNOPAction::ToStatement() const
{
	return CStatement(StatementType::NOP);
}