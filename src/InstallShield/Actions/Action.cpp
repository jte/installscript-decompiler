#include "Action.h"
#include "InternalFuncCallAction.h"
#include "AssignAction.h"
#include "FuncPrologAction.h"
#include "BinaryAction.h"
#include "UnaryAction.h"
#include "IfAction.h"
#include "GotoAction.h"
#include "IScript.h"
#include "ReturnAction.h"
#include "EndFuncAction.h"
#include "PropPutAction.h"
#include "PropGetAction.h"
#include "NOPAction.h"
#include "AddressOfAction.h"
#include "AbortAction.h"
#include "ExitAction.h"
#include "GetByteAction.h"
#include "SetByteAction.h"
#include "DLLFuncCallAction.h"
#include "PropPutRefAction.h"
#include "TryAction.h"
#include "EndTryAction.h"
#include "EndCatchAction.h"
#include "UseDLLAction.h"
#include "UnUseDLLAction.h"
#include "BindVariableAction.h"
#include "AddressOfWideAction.h"
#include "StrLengthCharsAction.h"
#include "StrSubAction.h"
#include "StrFindAction.h"
#include "StrCompareAction.h"
#include "StrToNumAction.h"
#include "NumToStrAction.h"
#include "HandlerAction.h"
#include "HandlerExAction.h"
#include "DoHandlerAction.h"
#include "ResizeAction.h"
#include "SizeofAction.h"

CAction::CAction(CIScript* script) :
	m_script(script)
{

}

void CAction::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_actionId);
	filePtr.Read(m_numOperands);
}

std::ostream& operator<<(std::ostream& out, const CAction& o)
{
	o.print(out);
	return out;
}

CAction* CAction::FindFactory(size_t id, CIScript* script, StreamPtr& fileptr)
{
	switch (id)
	{
	case 1: return new CNOPAction(script, fileptr);
	case 2: return new CAbortAction(script, fileptr);
	case 3: return new CExitAction(script, fileptr);
	case 4: return new CIfAction(script, fileptr);
	case 5: return new CGotoAction(script, fileptr);
	case 6: return new CAssignAction(script, fileptr);
	case 7: return new CBinaryAction<BinaryExprType::BinAdd>(script, fileptr);
	case 8: return new CBinaryAction<BinaryExprType::BinMod>(script, fileptr);
	case 9: return new CBinaryAction<BinaryExprType::BinLT>(script, fileptr);
	case 10: return new CBinaryAction<BinaryExprType::BinGT>(script, fileptr);
	case 11: return new CBinaryAction<BinaryExprType::BinLTE>(script, fileptr);
	case 12: return new CBinaryAction<BinaryExprType::BinGTE>(script, fileptr);
	case 13: return new CBinaryAction<BinaryExprType::BinEq>(script, fileptr);
	case 14: return new CBinaryAction<BinaryExprType::BinNEq>(script, fileptr);
	case 15: return new CBinaryAction<BinaryExprType::BinSub>(script, fileptr);
	case 16: return new CBinaryAction<BinaryExprType::BinMul>(script, fileptr);
	case 17: return new CBinaryAction<BinaryExprType::BinDiv>(script, fileptr);
	case 18: return new CBinaryAction<BinaryExprType::BitAnd>(script, fileptr);
	case 19: return new CBinaryAction<BinaryExprType::BitOr>(script, fileptr);
	case 20: return new CBinaryAction<BinaryExprType::BitXor>(script, fileptr);
	case 21: return new CUnaryAction<'~'>(script, fileptr);
	case 22: return new CBinaryAction<BinaryExprType::BitShl>(script, fileptr);
	case 23: return new CBinaryAction<BinaryExprType::BitShr>(script, fileptr);
	case 24: return new CBinaryAction<BinaryExprType::LogAnd>(script, fileptr);
	case 25: return new CBinaryAction<BinaryExprType::LogOr>(script, fileptr);
	case 26: return new CAddressOfAction(script, fileptr);
	case 27: return new CUnaryAction<'*'>(script, fileptr);
	case 28: return new CUnaryAction<'.'>(script, fileptr);
	case 29: return new CSetByteAction(script, fileptr);
	case 30: return new CGetByteAction(script, fileptr);
	//case 31: non-existant in 2016
	case 32: return new CDLLFuncCallAction(script, fileptr);
	case 33: return new CInternalFuncCallAction(script, fileptr);
	case 34: return new CFuncPrologAction(script, fileptr);
	case 35: return new CReturnAction(script, fileptr);
	case 36: return new CReturnAction(script, fileptr);
	case 37: return new CReturnAction(script, fileptr);
	case 38: return new CEndFuncAction(script, fileptr);
	case 39: return new CNOPAction(script, fileptr);
	case 40: return new CStrLengthCharsAction(script, fileptr);
	case 41: return new CStrSubAction(script, fileptr);
	case 42: return new CStrFindAction(script, fileptr);
	case 43: return new CStrCompareAction(script, fileptr);
	case 44: return new CStrToNumAction(script, fileptr);
	case 45: return new CNumToStrAction(script, fileptr);
	case 46: return new CHandlerAction(script, fileptr);
	case 47: return new CHandlerExAction(script, fileptr);
	case 48: return new CDoHandlerAction(script, fileptr);
	case 49: return new CResizeAction(script, fileptr);
	case 50: return new CSizeofAction(script, fileptr);
	case 51: return new CPropPutAction(script, fileptr);
	case 52: return new CPropPutRefAction(script, fileptr);
	case 53: return new CPropGetAction(script, fileptr);
	case 54: return new CTryAction(script, fileptr);
	case 55: return new CEndTryAction(script, fileptr);
	case 56: return new CEndCatchAction(script, fileptr);
	case 57: return new CUseDLLAction(script, fileptr);
	case 58: return new CUnUseDLLAction(script, fileptr);
	case 59: return new CBindVariableAction(script, fileptr);
	case 60: return new CAddressOfWideAction(script, fileptr);
	default: break;
	}
	throw std::runtime_error(std::string("No handler for action id ") + std::to_string(id) + std::string(" found"));
}
