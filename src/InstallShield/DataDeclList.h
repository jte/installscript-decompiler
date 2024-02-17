#pragma once

#include "StreamPtr.h"
#include "ActionFile/ActionFile.h"
#include <vector>

enum class SymFlags : uint16_t
{
	// check which are actually used
	vbEmpty =	0,
	vbNull	= 1,
	vbInteger =	2,
	vbLong =	3,
	vbSingle =	4,
	vbDouble =	5,
	vbCurrency =	6,
	vbDate =	7,
	vbString =	8,
	vbObject = 9,
	vbError =	10,
	vbBoolean =	11,
	vbVariant =	12,
	vbDataObject =	13,
	vbDecimal =	14,
	vbByte =	17,
	vbLongLong =	20,
	vbUserDefinedType =	36,
	vbArray =	0x2000,
};

struct ObjectVarInfo
{
	SymFlags flags;
	union
	{
		uint16_t elemCount; // for array
		int16_t typedefId; // for struct
	};
};

struct StringVarInfo
{
	uint16_t varId;
	uint16_t stringSize;
};

class CDataDeclList
{
public:
	void Parse(StreamPtr& ptr);
	void ParseGlobalForOBS(const std::vector<uint8_t>& script, const ActionFileHeaderOBS& hdr);
	friend std::ostream& operator<<(std::ostream& out, const CDataDeclList& o);
	uint16_t GetNumNumbers() const
	{
		return m_numNumbers;
	}
	uint16_t GetNumStrings() const
	{
		return m_numStrings;
	}
	const std::vector<StringVarInfo> GetStringTable() const
	{
		return m_stringTable;
	}
	const std::vector<ObjectVarInfo> GetObjectTable() const
	{
		return m_objectTable;
	}
protected:
	void ParseObjectTable(StreamPtr& ptr);
	void ParseStringTable(StreamPtr& ptr);
	void ParseStringTableOBS(StreamPtr& ptr);
	virtual void print(std::ostream& os) const;
private:
	uint16_t m_numNumbers = 0;
	uint16_t m_numStrings = 0;
	std::vector<ObjectVarInfo> m_objectTable;
	std::vector<StringVarInfo> m_stringTable;
};

std::ostream& operator<<(std::ostream& os, const std::vector<uint32_t>& vec);