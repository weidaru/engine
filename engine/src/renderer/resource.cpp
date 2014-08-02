#include "resource.h"

namespace {

unsigned int Resource::id = 0;

Resource::Resource () : id(++GetCurrentID()) {}

virtual Resource::~Resource() {}

unsigned int Resource::GetID() {
	return id;
}

std::string Resource::GetName() {
	return name;
}

Resource & Resource::SetName(const std::string &new_name) {
	name = new_name;
	return *this;
}

unsigned int & Resource::GetCurrentID() {
	static unsigned int current_id = 0;
	return current_id;
}

}

