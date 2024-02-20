#include "FuncPrologAction.h"

namespace oldis
{

void CFuncPrologAction::print(std::ostream& os) const
{
	os << "FuncProlog ";
}

CFuncPrologAction::CFuncPrologAction(CIScript* script, StreamPtr& filePtr) :
	CAction(script)
{
	Parse(filePtr);
	uint16_t Reserved;
	filePtr.Read(Reserved);
	uint16_t stringTable_count;
	filePtr.Read(stringTable_count);
	filePtr.ReadArray(m_stringTable, stringTable_count);
	uint16_t zeros;
	filePtr.Read(zeros);
	filePtr.Read(m_numNumbers);
	filePtr.Read(zeros);
}


};