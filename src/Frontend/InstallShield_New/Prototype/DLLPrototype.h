#pragma once

#include "Prototype.h"
#include "StreamPtr.h"

namespace newis
{

class CDLLPrototype : public CPrototype
{
protected:
	std::string m_dllName;
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

};