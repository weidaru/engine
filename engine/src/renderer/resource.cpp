#include "resource.h"

namespace s2 {

Resource::Resource () : id(++GetCurrentID()) {}

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

unsigned int & Resource::GetCurrentID() {
	static unsigned int current_id = 0;
	return current_id;
}

}

