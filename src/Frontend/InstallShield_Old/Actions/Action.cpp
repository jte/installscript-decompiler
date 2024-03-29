#include "Action.h"
#include "InternalFuncCallAction.h"
#include "FuncPrologAction.h"
#include "ReturnEmptyAction.h"
#include "IScript.h"
#include "AssignAction.h"
#include "BinaryAction.h"
#include "IfAction.h"
#include "ReturnFromProgramAction.h"
#include "UnaryAction.h"
#include "NopAction.h"
#include "ReturnAction.h"
#include "GotoAction.h"
#include "LoadStringConstantAction.h"
#include "PropGetAction.h"
#include "PropPutAction.h"
#include "PlaceholderAction.h"
#include "EnableAction.h"
#include "DisableAction.h"
#include "SetTitleAction.h"

namespace oldis
{

CAction::CAction(CIScript* script) :
	m_script(script), m_actionId(-1)
{

}

void CAction::Parse(StreamPtr& filePtr)
{
	filePtr.Read(m_actionId);
}

CAction* CAction::FindFactory(size_t id, CIScript* script, StreamPtr& fileptr)
{
	switch (id)
	{
	case 0x01: return new CEnableAction(script, fileptr);
	case 0x02: return new CDisableAction(script, fileptr);
	case 0x04: return new CSetTitleAction(script, fileptr);
	case 0x13: return new CAssignAction(script, fileptr);
	case 0x21: return new CAssignAction(script, fileptr);
	case 0x22: return new CIfAction(script, fileptr);
	case 0x2b: return new CReturnFromProgramAction(script, fileptr);
	case 0x2c: return new CGotoAction(script, fileptr);
	case 0xb5: return new CInternalFuncCallAction(script, fileptr);
	case 0xb6: return new CFuncPrologAction(script, fileptr);
	case 0xb7: return new CReturnAction(script, fileptr);
	case 0xb8: return new CReturnEmptyAction(script, fileptr);
	case 0x112: return new CLoadStringConstantAction(script, fileptr);
	case 0x119: return new CBinaryAction<BinaryExprType::BinAdd>(script, fileptr);
	case 0x11a: return new CBinaryAction<BinaryExprType::BinSub>(script, fileptr);
	case 0x11b: return new CBinaryAction<BinaryExprType::BinMul>(script, fileptr);
	case 0x11c: return new CBinaryAction<BinaryExprType::BinDiv>(script, fileptr);
	case 0x11d: return new CBinaryAction<BinaryExprType::BitAnd>(script, fileptr);
	case 0x11e: return new CBinaryAction<BinaryExprType::BitOr>(script, fileptr);
	case 0x11f: return new CBinaryAction<BinaryExprType::BitXor>(script, fileptr);
	case 0x120: return new CUnaryAction<'~'>(script, fileptr);
	case 0x121: return new CBinaryAction<BinaryExprType::BitShl>(script, fileptr);
	case 0x122: return new CBinaryAction<BinaryExprType::BitShr>(script, fileptr);
	case 0x123: return new CBinaryAction<BinaryExprType::BinMod>(script, fileptr);
	case 0x126: return new CBinaryAction<BinaryExprType::LogOr>(script, fileptr);
	case 0x127: return new CBinaryAction<BinaryExprType::LogAnd>(script, fileptr);
	case 0x128: return new CBinaryAction<BinaryExprType::CodeDefined>(script, fileptr);
	case 0x12f: return new CNopAction(script, fileptr);
	case 0x181: return new CPropGetAction(script, fileptr);
	case 0x183: return new CPropPutAction(script, fileptr);
	default: return new CPlaceholderAction(script, fileptr);
	}
	throw std::runtime_error(std::string("No handler for action id ") + std::to_string(id) + std::string(" found"));
}

};