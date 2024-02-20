#include "Action.h"
#include "InternalFuncCallAction.h"
#include "FuncPrologAction.h"
#include "ReturnEmptyAction.h"
#include "IScript.h"

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
	case 43: return new CReturnEmptyAction(script, fileptr);
		case 181: return new CInternalFuncCallAction(script, fileptr);
		case 182: return new CFuncPrologAction(script, fileptr);
	default: break;
	}
	throw std::runtime_error(std::string("No handler for action id ") + std::to_string(id) + std::string(" found"));
}

};