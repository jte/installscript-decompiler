#pragma once

#include <cstdint>
#include <vector>
#include "Variable.h"
#include "IArgument.h"

enum class StatementType : uint32_t
{
	NOP,
	Return,
	Abort,
	Exit,
	If,
	Goto,
	Assign,
	UnaryComplement = '~',
	UnaryIndirection = '*',
	UnaryMember = '.',
	FuncProlog,
	FuncEpilog,
	PropPutRef,
	PropGet,
	Try,
	EndTry,
	EndCatch,
	UseDLL,
	UnUseDLL,
	BindVariable,
	DLLFuncCall
};

class CExpression;

class CStatement
{
private:
	StatementType m_type;
	std::vector<CExpression*> m_exprs;
	class CBasicBlock* m_owner;
public:
	CStatement() {}
	CStatement(StatementType type);// actions without args
	CStatement(StatementType type, std::vector<CExpression*> exprs);
	StatementType GetType() const
	{
		return m_type;
	}
	void SetOwner(CBasicBlock* owner)
	{
		m_owner = owner;
	}
	std::vector<CExpression*> GetExpressions() const
	{
		return m_exprs;
	}
	friend std::ostream& operator<<(std::ostream& out, const CStatement& o);
};