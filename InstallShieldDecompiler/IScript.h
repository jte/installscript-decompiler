#pragma once

#include <vector>
#include <algorithm>
#include "StreamPtr.h"
#include "Struct.h"
#include "ActionFile.h"
#include "Prototype.h"
#include "DataDeclList.h"

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

class CIScript
{
public:
	typedef std::vector<CAction*> ScriptBasicBlock;
	struct Function
	{
		Function(CPrototype* proto) : 
			prototype(proto)
		{}
		Function() : 
			prototype(nullptr)
		{}
		CDataDeclList dataDeclList;
		CPrototype* prototype;
		std::vector<ScriptBasicBlock> bbs;
	};
	Function& GetFnById(size_t id)
	{
		return m_fns.at(id);
	}
	Function& GetFnByBBId(size_t addr)
	{
		auto fn = std::find_if(m_fns.begin(), m_fns.end(), [addr](const Function& fn) 
		{ 
			return fn.prototype->GetBBId() == addr; 
		});
		if (fn == m_fns.end())
			throw std::runtime_error("Function not found");
		return *fn;
	}
	std::vector<Function> GetFns() const
	{
		return m_fns;
	}
private:
	std::vector<Function> m_fns;
	std::vector<uint8_t> m_script;
	StreamPtr m_streamPtr;
	std::vector<CStruct> m_structs;
	ActionFileHeader m_header = {};
	std::vector<ExternRecord> m_externs;

	void ReadHeader();
	void ReadStructs(uint32_t tableOffset);
	void ReadPrototypes(uint32_t tableOffset);
	void ReadBBs(uint32_t tableOffset);
	void Read();
	static void DecryptBuffer(uint32_t* seed, std::vector<uint8_t> buffer, char key);
	void ReadExternTable(uint32_t tableOffset);
protected:
	void ReadVariantTable(StreamPtr& filePtr);
	void ReadSymFlagTable(StreamPtr& filePtr);

	int32_t m_functionId;
public:
	CIScript(const std::vector<uint8_t>& script);
	const CStruct& GetStruct(size_t id) const;
	friend std::ostream& operator<<(std::ostream& out, const CIScript& o);
	void PrintPrototypes();
	std::vector<ExternRecord> GetExterns() const
	{
		return m_externs;
	}
	friend class CEndFuncAction;
};