#pragma once

#include <vector>
#include <algorithm>
#include "StreamPtr.h"
//#include "Struct/Struct.h"
//#include "ActionFile/ActionFile.h"
//#include "Prototype/Prototype.h"
#include "DataDeclList.h"
//#include "ISBasicBlock.h"
#include "HeaderKind.h"
#include "Frontend.h"
#include "ScriptStruct.h"
#include "ScriptFunction.h"
#include "ScriptPrototype.h"
#include "ScriptExtern.h"

namespace oldis
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
	std::vector<ScriptFunction> GetFns() const override
	{
		return m_fns;
	}
	void print(std::ostream& os) const override;
	const CDataDeclList& GetGlobalDeclList() const override
	{
		return CDataDeclList();
	//	return m_globalDeclList;
	}
	CIScript(const std::vector<uint8_t>& script, HeaderKind hdrKind);
	const ScriptStruct* GetStruct(size_t id) const;
	const std::vector<ScriptStruct>& GetStructs() const
	{
		return m_structs;
	}
	HeaderKind GetHeaderKind() const
	{
		return m_hdrKind;
	}

	std::vector<ScriptExtern> GetExterns() const override
	{
		return m_externs;
	}
private:
	struct SLoadableStringEntry
	{
		uint16_t stringId;
		std::string name;
	};
	struct SLoadableNumberEntry
	{
		uint16_t numberId;
		std::string name;
	};
	std::vector<ScriptFunction> m_fns;
	std::vector<uint8_t> m_script;
	std::vector<ScriptStruct> m_structs;
	std::vector<ScriptExtern> m_externs;
	std::vector<SLoadableStringEntry> m_loadableStrings;
	std::vector<SLoadableNumberEntry> m_loadableNumbers;

	StreamPtr m_streamPtr;
	HeaderKind m_hdrKind;
	uint16_t m_numEvents; // really numBBs
	uint16_t m_globalNumberCount; // count of number variables

	void Read();
	
	void ReadHeader();
	void ReadGlobalStringTable(StreamPtr& ptr);
	void ReadLoadableStringTable(StreamPtr& ptr);
	void ReadLoadableNumberTable(StreamPtr& ptr);
	void ReadStructs(StreamPtr& ptr);
	void ReadPrototypes(StreamPtr& ptr);
	void ReadEvents(StreamPtr& ptr);
};

};