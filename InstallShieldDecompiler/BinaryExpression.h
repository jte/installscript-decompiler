#pragma once

#include "Expression.h"

enum class BinaryExprType
{
	BinAdd,
	BinSub,
	BinMod,
	BinLT,
	BinGT,
	BinLTE,
	BinGTE,
	BinEq,
	BinNEq,
	BinMul,
	BinDiv,
	BitAnd,
	BitOr,
	BitXor,
	BitShl,
	BitShr,
	LogAnd,
	LogOr
};

class CBinaryExpression : public CExpression
{
private:
	BinaryExprType m_type;
	CExpression* m_first;
	CExpression* m_second;

	constexpr const char* BinaryExprTypeToString(BinaryExprType type);
protected:
	void print(std::ostream& out);
public:
	CBinaryExpression(BinaryExprType type, CExpression* first, CExpression* second) :
		m_type(type), m_first(first), m_second(second)
	{}
};