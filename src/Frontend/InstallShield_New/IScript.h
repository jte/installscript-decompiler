#pragma once

#include <vector>
#include <algorithm>
#include "StreamPtr.h"
#include "Struct/Struct.h"
#include "ActionFile/ActionFile.h"
#include "Prototype/Prototype.h"
#include "DataDeclList.h"
#include "ISBasicBlock.h"
#include "HeaderKind.h"
#include "Frontend.h"
#include "ScriptPrototype.h"
#include "ScriptStruct.h"

namespace newis
{

//class CPrototype;
class CAction;

struct ExternRecord
{
	//		// 1 -> object
		// 2 -> ?
		// 3 -> ?
	int type;
	std::string name;
	int16_t address;
	ExternRecord(int type, std::string name, int16_t address) :
		type(type), name(name), address(address)
	{}
};

class CIScript : public ::CFrontend
{
public:
	ScriptFunction& GetFnById(size_t id)
	{
		return m_fns.at(id);
	}
	ScriptFunction& GetFnByBBId(size_t addr)
	{
		auto fn = std::find_if(m_fns.begin(), m_fns.end(), [addr](const ScriptFunction& fn)
			{
				return fn.prototype->GetBBId() == addr;
			});
		if (fn == m_fns.end())
			throw std::runtime_error("Function not found");
		return *fn;
	}
	std::vector<ScriptFunction> GetFns() const
	{
		return m_fns;
	}
	const CDataDeclList& GetGlobalDeclList() const override
	{
		return m_globalDeclList;
	}
	CIScript(const std::vector<uint8_t>& script, HeaderKind hdrKind);
	const ScriptStruct* GetStruct(size_t id) const override;
	const std::vector<ScriptStruct>& GetStructs() const override
	{
		return m_structs;
	}
	HeaderKind GetHeaderKind() const
	{
		return m_hdrKind;
	}

	void print(std::ostream& os) const override;
	
	std::vector<ExternRecord> GetExterns() const
	{
		return m_externs;
	}

private:
	std::vector<ScriptFunction> m_fns;
	std::vector<uint8_t> m_script;
	StreamPtr m_streamPtr;
	std::vector<ScriptStruct> m_structs;
	ActionFileHeaderOBS m_headerOBS = {};
	ActionFileHeaderaLuZ m_headerALUZ = {};
	std::vector<ExternRecord> m_externs;
	HeaderKind m_hdrKind;
	CDataDeclList m_globalDeclList;

	void ReadBBsOBS(uint32_t tableOffset);
	void ReadBBsALUZ(uint32_t tableOffset);
	void ReadHeader();
	void ReadStructs(uint32_t tableOffset);
	void ReadPrototypes(uint32_t tableOffset);
	void ReadAddressResolve(uint32_t tableOffset);
	void Read();

	void ReadExternTable(uint32_t tableOffset);
};

};