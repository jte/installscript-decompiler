#include "GotoAction.h"
#include <cassert>
#include <string>
#include "NumConst.h"

namespace newis
{

void CGotoAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 1);
	os << "goto " << std::dec << GetLabelOffset() << std::hex << ';';
}

int32_t CGotoAction::GetLabelOffset() const
{
	CNumConst* offset = dynamic_cast<CNumConst*>(m_arguments[0]);
	return static_cast<int32_t>(offset->GetConst());
}

};
