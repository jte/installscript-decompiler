#include "FuncPrologAction.h"
#include "StreamPtr.h"
#include <string>
#include <cassert>

namespace newis
{

void CFuncPrologAction::print(std::ostream& os) const
{
	os << "FuncProlog " << std::hex << m_tableOffset;
}

CFuncPrologAction::CFuncPrologAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script), m_tableOffset(0)
{
	Parse(filePtr);
	char mark = 7; // NumConst
	filePtr.Read(mark);
	assert(mark == 7);
	StreamPtr table(filePtr);
	filePtr.Read(m_tableOffset);
	table.JumpTo(table.GetOffset() + m_tableOffset);
	m_declList.Parse(table);
}

};