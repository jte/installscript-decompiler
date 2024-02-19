#include "Variable.h"

std::ostream& operator<<(std::ostream& out, const CVariable& o)
{
	o.print(out);
	return out;
}