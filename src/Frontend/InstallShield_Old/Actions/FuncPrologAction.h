#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "DataDeclList.h"

namespace oldis
{

class CFuncPrologAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	std::vector<uint16_t> m_stringTable;
	CDataDeclList m_declList;
public:
	CFuncPrologAction(CIScript* script, StreamPtr& filePtr);
	CDataDeclList GetDataDeclList() const
	{
		return m_declList;
	}
};

};
