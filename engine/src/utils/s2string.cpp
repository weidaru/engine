#include "s2string.h"

#include <stdarg.h>
#include <stdio.h>

namespace s2 {

void S2StringFormat(s2string *str, const char *pattern, ...) {
	va_list args;
    va_start(args, pattern);
	char buf[1024];
	vsprintf_s(buf, 1024, pattern, args);
	*str = buf;
    va_end(args);
}

}