#include "UseDLLAction.h"
#include <cassert>
#include <string>

namespace newis
{

void CUseDLLAction::print(std::ostream& os) const
{
	assert(m_arguments.size() == 1);
}

};