#include "s2string.h"

#include <stdarg.h>
#include <stdio.h>

namespace s2 {

void S2StringFormat(s2string *str, const char *pattern, ...) {
	va_list args;
    va_start(args, pattern);
	char buf[512];
	sprintf_s(buf, 512, pattern, args);
	*str = buf;
    va_end(args);
}

}