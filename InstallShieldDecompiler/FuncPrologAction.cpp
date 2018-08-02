#include "FuncPrologAction.h"
#include "StreamPtr.h"
#include <string>
#include <cassert>

void CFuncPrologAction::print(std::ostream& os) const
{
	os << "FuncProlog " << std::hex << m_tableOffset;
}

CFuncPrologAction::CFuncPrologAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
	char mark = 7;
	filePtr.Read(mark);
	assert(mark == 7);
	filePtr.Read(m_tableOffset);
}

CStatement CFuncPrologAction::ToStatement() const
{
	return CStatement(StatementType::FuncProlog);
}