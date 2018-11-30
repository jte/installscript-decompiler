#pragma once

#include "Prototype.h"

class CInternalPrototype : public CPrototype
{
protected:
	void print(std::ostream& os) const;
public:
	CInternalPrototype(StreamPtr& filePtr);
	void Parse(StreamPtr& filePtr);
	~CInternalPrototype() = default;
	static CPrototype* Make(StreamPtr& filePtr)
	{
		return new CInternalPrototype(filePtr);
	}
};