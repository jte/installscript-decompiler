#include "StreamPtr.h"

std::ostream& operator<<(std::ostream& os, const char* str) {
	if (str != nullptr) {
		while (*str) {
			os.put(*str);
			++str;
		}
	}
	return os;
}