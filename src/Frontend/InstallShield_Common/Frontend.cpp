#include "Frontend.h"

std::ostream& operator<<(std::ostream& out, const CFrontend& o)
{
	o.print(out);
	return out;
}