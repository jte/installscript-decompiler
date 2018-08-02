#pragma once

#include "ActionWithArgs.h"
#include <cassert>

template <int Sym>
class CUnaryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const
	{
		assert(m_arguments.size() == 3);
		os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << Sym << ' ' << *(m_arguments[2]);
	}
	CStatement ToStatement() const
	{
		return CStatement(static_cast<StatementType>(Sym), CExpression::FromScript(m_arguments));
	}
public:
	CUnaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};