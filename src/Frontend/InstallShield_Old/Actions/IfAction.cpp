#include "IfAction.h"
#include <cassert>
#include <string>
#include "NumConst.h"

namespace oldis
{

CIfAction::CIfAction(CIScript* script, StreamPtr& filePtr) :
	CActionWithArgs(script, filePtr)
{
	filePtr.Read(m_label);
	filePtr.Read(m_realAddress);
	uint8_t _padding1; // match: 0x95
	filePtr.Read(_padding1);
	m_numOperands = 1;
	m_arguments.resize(m_numOperands);
	ParseArguments(filePtr);
	std::vector<char> _padding2; // match: "A\0\0\0\0"
	filePtr.ReadArray(_padding2, 5);
}

void CIfAction::print(std::ostream& os) const
{
	os << "IF (label: " << (int)m_label << ", realAddress: " << m_realAddress << ") " << *(m_arguments[0]);
}


};