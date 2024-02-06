#pragma once

#include "Action.h"
#include "StreamPtr.h"
#include "CNumConst.h"
#include "DataDeclList.h"

class CFuncPrologAction : public CAction
{
protected:
	void print(std::ostream& os) const override;
	uint32_t m_tableOffset;
	CDataDeclList m_declList;
public:
	CFuncPrologAction(CIScript* script, StreamPtr& filePtr);
	CDataDeclList GetDataDeclList() const
	{
		return m_declList;
	}
};