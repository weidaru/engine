#ifndef S2_STRING_H_
#define S2_STRING_H_

#include <string>

namespace s2 {

typedef std::string s2string;

//This now uses 1024-byte stack. TODO: Refactor if necessary.
void S2StringFormat(s2string *str, const char *pattern, ...);

}

#endif 		//S2_STRING_H_