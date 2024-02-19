#include "Action.h"

std::ostream& operator<<(std::ostream& out, const CAction& o)
{
	o.print(out);
	return out;
}