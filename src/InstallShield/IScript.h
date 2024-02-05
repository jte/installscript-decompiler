#pragma once

#include <vector>
#include <algorithm>
#include "StreamPtr.h"
#include "Struct/Struct.h"
#include "ActionFile/ActionFile.h"
#include "Prototype/Prototype.h"
#include "DataDeclList.h"
#include "ISBasicBlock.h"

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
		{
		}
		Function() : 
			prototype(nullptr)
		{
		}
		CDataDeclList dataDeclList;
		CPrototype* prototype;
		std::vector<size_t> labels;
		std::vector<ISBasicBlock> bbs;
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
	void ReadAddressResolve(uint32_t tableOffset);
	void Read();
	
	void ReadExternTable(uint32_t tableOffset);
protected:
	void ReadVariantTable(StreamPtr& filePtr);
	void ReadSymFlagTable(StreamPtr& filePtr);
public:
	static void DecryptBuffer(uint32_t* seed, std::vector<uint8_t> buffer, char key);
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