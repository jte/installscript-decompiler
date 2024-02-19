#include "IScript.h"
//#include "Prototype/Prototype.h"
//#include "Prototype/InternalPrototype.h"
//#include "Prototype/DLLPrototype.h"
//#include "ActionFile/ActionFile.h"
#include "Actions/Action.h"
//#include "Actions/FuncCallAction.h"
//#include "Actions/FuncPrologAction.h"
//#include "Actions/EndFuncAction.h"
//#include "Actions/IfAction.h"
//#include "Actions/GotoAction.h"
#include <iostream>
#include <string.h>

namespace oldis
{

CIScript::CIScript(const std::vector<uint8_t>& script, HeaderKind hdrKind) :
	m_script(script), m_streamPtr(m_script), m_hdrKind(hdrKind)
{
	Read();
}

void CIScript::ReadHeader()
{
	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
		break;
	case HeaderKind::aLuZ:
		break;
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::Unrecognized:
		break;
	}

	m_streamPtr = StreamPtr(m_script);
}


void CIScript::ReadPrototypes(uint32_t tableOffset)
{
}

void CIScript::ReadStructs(uint32_t tableOffset)
{
}

void CIScript::ReadBBsOBS(uint32_t tableOffset)
{
}

void CIScript::ReadBBsALUZ(uint32_t tableOffset)
{
}

const ScriptStruct* CIScript::GetStruct(size_t id) const
{
	return &m_structs.at(id);
}

void CIScript::ReadExternTable(uint32_t tableOffset)
{
}

void CIScript::Read()
{
	ReadHeader();

	switch (m_hdrKind)
	{
	case HeaderKind::OBS:
		break;
	case HeaderKind::aLuZ:
		break;
	case HeaderKind::kUtZ:
		break;
	case HeaderKind::Unrecognized:
		break;
	}
}

void CIScript::print(std::ostream& os) const
{
	/**for (const auto& fn : o.m_fns)
	{
		if (fn.prototype->GetIsExported() || fn.bbs.size() != 0)
		{
			out << *(fn.prototype);
			out << fn.dataDeclList;
			for (const auto& bb : fn.bbs)
			{
				for (const auto& act : bb.GetActions())
				{
					out << std::hex << bb.GetBBId() << std::dec << ": ";
					out << *act << std::endl;
				}
			}
			out << "\n\n" << std::endl;
		}
	}*/
}

};