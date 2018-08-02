#pragma once

#include "Prototype.h"

class CInternalPrototype : public CPrototype
{
protected:
	uint16_t m_address;
	void print(std::ostream& os) const;
public:
	CInternalPrototype(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	~CInternalPrototype() = default;
	uint16_t GetAddress() const
	{
		return m_address;
	}
	static CPrototype* Make(StreamPtr& filePtr)
	{
		return new CInternalPrototype(filePtr);
	}
};