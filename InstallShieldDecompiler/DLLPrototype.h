#pragma once

#include "Prototype.h"
#include "StreamPtr.h"

class CDLLPrototype : public CPrototype
{
protected:
	std::string m_dllName;
	uint16_t m_address;
	void print(std::ostream& os) const;
public:
	CDLLPrototype(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	std::string GetDLLName() const;
	~CDLLPrototype() = default;
	static CPrototype* Make(StreamPtr& filePtr)
	{
		return new CDLLPrototype(filePtr);
	}
};