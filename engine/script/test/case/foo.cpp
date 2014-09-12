#include "foo.h"

//[[TypeInfo]]//
struct Bar {
	long a;
	Foo * foo_ptr;
	Foo &foo_ref;
};