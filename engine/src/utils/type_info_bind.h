#ifndef TYPE_INFO_BIND_H_
#define TYPE_INFO_BIND_H_

namespace s2 {
//They are small, same to get included.
static const char *null_typename = "null";
static const char *pointer_typename = "pointer";
static const char *reference_typename = "reference";

template <typename T>
struct TypeInfoBind {
	//If this has link error, you are trying to access an unbinded typeinfo.
	static s2string GetName();
};

template <typename T>
struct TypeInfoBind<T *> {
	static s2string GetName() {
		return pointer_typename;
	}
};

template <typename T>
struct TypeInfoBind<T &> {
	static s2string GetName() {
		return reference_typename;
	}
};

}

#endif		//TYPE_INFO_BIND_H_