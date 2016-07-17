#ifndef S2_STRING_H_
#define S2_STRING_H_

#include <string>
#include <utility>
#include <cppformat/format.h>

namespace s2 {

typedef std::string s2string;
typedef std::wstring s2wstring;

template<typename... Params>
void S2StringFormat(s2string *str, const char *pattern, Params&&... parameters) {
	*str = fmt::sprintf(pattern, std::forward<Params>(parameters)...);
}

}

#endif 		//S2_STRING_H_