#include "resource.h"

namespace s2 {

Resource::Resource () : id(GetCurrentID()++), name("") {}

Resource::~Resource() {}

unsigned int Resource::GetID() const {
	return id;
}

s2string Resource::GetName() const {
	return name;
}

Resource & Resource::SetName(const s2string &new_name) {
	name = new_name;
	return *this;
}

s2string Resource::GetIDAndName() const {
	s2string result;
	S2StringFormat(&result, "[%d (%s)]", id, name.c_str());
	return result;
}

unsigned int & Resource::GetCurrentID() {
	static unsigned int current_id = 0;
	return current_id;
}

}

