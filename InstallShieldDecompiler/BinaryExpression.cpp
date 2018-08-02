#include "BinaryExpression.h"
#include <ostream>

void CBinaryExpression::print(std::ostream& out)
{
	out << m_first << ' ' << BinaryExprTypeToString(m_type) << ' ' << m_second;
}

constexpr const char* CBinaryExpression::BinaryExprTypeToString(BinaryExprType type)
{
	switch (type)
	{
	case BinaryExprType::BinAdd: return "+";
	case BinaryExprType::BinSub: return "-";
	case BinaryExprType::BinMod: return "%";
	case BinaryExprType::BinLT: return "<";
	case BinaryExprType::BinGT: return ">";
	case BinaryExprType::BinLTE: return "<=";
	case BinaryExprType::BinGTE: return ">=";
	case BinaryExprType::BinEq: return "==";
	case BinaryExprType::BinNEq: return "!=";
	case BinaryExprType::BinMul: return "*";
	case BinaryExprType::BinDiv: return "/";
	case BinaryExprType::BitAnd: return "&";
	case BinaryExprType::BitOr: return "|";
	case BinaryExprType::BitXor: return "^";
	case BinaryExprType::BitShl: return "<<";
	case BinaryExprType::BitShr: return ">>";
	case BinaryExprType::LogAnd: return "&";
	case BinaryExprType::LogOr: return "|";
	default: return "INVALID";
	}
}