#pragma once

#include "ActionWithArgs.h"
#include "Parser/Expressions.h"

namespace oldis
{
	
template <BinaryExprType Sym>
class CBinaryAction : public CActionWithArgs
{
public:
	CBinaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{
		switch (m_actionId)
		{
		case 0x128:
			// Arguments layout:
			// DEST = LHS OP RHS
			// [0] -> DEST
			// [1] -> LHS
			// [2] -> OP
			// [3] -> RHS
			m_numOperands = 4;
			break;
		case 0x126:
		case 0x127:
		case 0x122:
		case 0x121:
		case 0x120:
		case 0x11f:
		case 0x11e:
		case 0x11d:
		case 0x11c:
		case 0x11b:
		case 0x11a:
		case 0x123:
		case 0x119:
			m_numOperands = 3;
			break;
		}
		m_arguments.resize(m_numOperands);
		ParseArguments(filePtr);
	}
protected:
	void print(std::ostream& os) const override
	{
		if (m_numOperands == 4)
		{
			os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << *(m_arguments[2]) << ' ' << *(m_arguments[3]);
		}
		else if (m_numOperands == 3)
		{
			os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << (int)Sym << ' ' << *(m_arguments[2]);
		}
	}

};

};
