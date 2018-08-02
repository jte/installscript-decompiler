#include "IArgument.h"

std::ostream & operator<<(std::ostream & out, const IArgument & o)
{
	o.print(out);
	return out;
}
