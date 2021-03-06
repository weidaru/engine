#ifndef TYPE_INFO_BIND_H_
#define TYPE_INFO_BIND_H_

namespace s2 {
//They are small, simple to get included.
static const char *kPointerTypename= "pointer";

template <typename T>
struct TypeInfoBind {
	//If this has link error, you are trying to access an unbinded typeinfo.
	static s2string GetName();
};

template <typename T>
struct TypeInfoBind<T *> {
	static s2string GetName() {
		return kPointerTypename;
	}
};

}

#endif		//TYPE_INFO_BIND_H_