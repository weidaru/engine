#include "foo.h"

//[[TypeInfo]]//
struct Bar {
	double a;
	Foo * foo_ptr;
	Foo &foo_ref;
};