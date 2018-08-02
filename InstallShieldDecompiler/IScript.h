#pragma once

#include <vector>
#include "StreamPtr.h"
#include "Struct.h"
#include "ActionFile.h"
#include "Optimizer.h"

class CPrototype;
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
private:
	
	std::vector<uint8_t> m_script;
	StreamPtr m_streamPtr;
	std::vector<CPrototype*> m_prototypes;
	std::vector<CStruct> m_structs;
	ActionFileHeader m_header = {};
	std::vector<ScriptBasicBlock> m_bbs;
	std::vector<size_t> m_bbSizes;
	std::vector<ExternRecord> m_externs;

	void ReadHeader();
	void ReadStructs(uint32_t tableOffset);
	void ReadPrototypes(uint32_t tableOffset);
	void ReadBBs(uint32_t tableOffset);
	void Read();
	static void DecryptBuffer(uint32_t* seed, std::vector<uint8_t> buffer, char key);
	void ReadVariablesTable(const StreamPtr& filePtr);
	void ReadExternTable(uint32_t tableOffset);
protected:
	void ReadVariantTable(StreamPtr& filePtr);
	void ReadSymFlagTable(StreamPtr& filePtr);

	int32_t m_functionId;
public:
	CIScript(const std::vector<uint8_t>& script);
	const CStruct& GetStruct(size_t id) const;
	const CPrototype* GetPrototype(size_t id) const;
	friend std::ostream& operator<<(std::ostream& out, const CIScript& o);
	size_t GetLastLabelId() const;
	void PrintPrototypes();
	int32_t GetCurrentFunctionId() const;
	std::vector<CPrototype*> GetPrototypes() const
	{
		return m_prototypes;
	}
	std::vector<ScriptBasicBlock> GetBasicBlocks() const
	{
		return m_bbs;
	}
	std::vector<ExternRecord> GetExterns() const
	{
		return m_externs;
	}
	friend class CEndFuncAction;
};