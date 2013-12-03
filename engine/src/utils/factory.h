#ifndef COMPONENT_FACTORY_H_
#define COMPONENT_FACTORY_H_

#include <string>
#include <map>
#include <assert.h>
#include <glog/logging.h>

template <typename T>
class Registration {
public:
	virtual ~Registration () {}
	virtual T * Create(void* data) = 0;
};

template <typename T>
class Factory {
private:
	typedef std::map<std::string, Registration<T> *> RegistrationMap;

public:
	static void Register(const std::string &name, Registration<T>* reg) {
		CHECK(getMap().find(name) == getMap().end()) << "prototype "<<name<<" has already been registered";
		getMap()[name] = reg;
	}

	static T * Create(const std::string &name, void *data = NULL) {
		if( getMap().find(name) == getMap().end()) {
			LOG(ERROR)<<"Unknown prototype registration: "<<name;
			return NULL;
		}
		return getMap()[name]->Create(data);
	}

private:
	static RegistrationMap & getMap() {
		static RegistrationMap map;
		return map;
	}
};

#include "factory.inl"

#endif