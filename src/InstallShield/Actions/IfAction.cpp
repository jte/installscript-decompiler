#include "IfAction.h"
#include <cassert>
#include <string>
#include "CNumConst.h"

void CIfAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 2);
	os << "If (else:" << this->GetElseBranchLabelOffset() << ") " << *(m_arguments[1]);
}

uint32_t CIfAction::GetElseBranchLabelOffset() const
{
	return dynamic_cast<CNumConst*>(m_arguments[0])->GetConst();
}
