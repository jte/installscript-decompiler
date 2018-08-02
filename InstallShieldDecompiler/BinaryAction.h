#pragma once

#include "ActionWithArgs.h"
#include <cassert>
#include "Statement.h"
#include "BinaryExpression.h"

template <int Sym>
class CBinaryAction : public CActionWithArgs
{
protected:
	void print(std::ostream& os) const
	{
		assert(m_arguments.size() == 3);
		os << *(m_arguments[0]) << " = " << *(m_arguments[1]) << ' ' << SymToString() << ' ' << *(m_arguments[2]);
	}
	std::string SymToString() const
	{
		std::string s;
		char arr[] = { Sym >> 24, Sym >> 16, Sym >> 8, Sym};
		if (arr[0]) s += arr[0];
		if (arr[1]) s += arr[1];
		if (arr[2]) s += arr[2];
		if (arr[3]) s += arr[3];
		return s;
	}
	constexpr BinaryExprType SymToBinaryExprType() const
	{
		switch (Sym)
		{
		case '+': return BinaryExprType::BinAdd;
		case '-': return BinaryExprType::BinSub;
		case '%': return BinaryExprType::BinMod;
		case '<': return BinaryExprType::BinLT;
		case '>': return BinaryExprType::BinGT;
		case '<=': return BinaryExprType::BinLTE;
		case '>=': return BinaryExprType::BinGTE;
		case '==': return BinaryExprType::BinEq;
		case '!=': return BinaryExprType::BinNEq;
		case '*': return BinaryExprType::BinMul;
		case '/': return BinaryExprType::BinDiv;
		case '&': return BinaryExprType::BitAnd;
		case '|': return BinaryExprType::BitOr;
		case '^': return BinaryExprType::BitXor;
		case '<<': return BinaryExprType::BitShl;
		case '>>': return BinaryExprType::BitShr;
		case '&&': return BinaryExprType::LogAnd;
		case '||': return BinaryExprType::LogOr;
		default: return (BinaryExprType)Sym;//static_assert(false); // unsupported operator
		}
	}
	CStatement ToStatement() const
	{
		BinaryExprType type = SymToBinaryExprType();

		std::vector<CExpression*> exprs;
		exprs.push_back(m_arguments[0]->ToExpression()); // destination
		CBinaryExpression* binExpr = new CBinaryExpression(type, m_arguments[1]->ToExpression(), m_arguments[2]->ToExpression());
		exprs.push_back(binExpr);
		return CStatement(StatementType::Assign, exprs);
	}
public:
	CBinaryAction(CIScript* script, StreamPtr& filePtr) :
		CActionWithArgs(script, filePtr)
	{}
};