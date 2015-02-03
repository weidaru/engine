#ifndef RESOURCE_CONFLICT_RESOLVER_H_
#define RESOURCE_CONFLICT_RESOLVER_H_

#include <map>
#include <vector>
#include <functional>

namespace s2 {

class Resource;

typedef std::vector<std::pair<unsigned int, Resource *> > BindingVec;

struct BindingMap {
	std::map<Resource *, unsigned int> map;
	std::vector<Resource *> vec;

	BindingMap(unsigned int size);

	bool Contains(unsigned int index);
	bool Contains(Resource *resource);

	BindingMap & Remove(unsigned int index);
	BindingMap & Remove(Resource *resource);

	BindingMap & Add(unsigned int index, Resource *resource);

	Resource * GetResource(unsigned int index);
	unsigned int GetIndex(Resource *);

	void Update(const std::vector<std::pair<unsigned int, Resource *> > &new_things);
};


struct ResolverClosure {
	typedef std::function<BindingVec()> DiscoverFunc;
	//@param (int active_index, Resource *resource)
	typedef std::function<void(int, Resource *)> ResolveFunc;

	ResolverClosure(unsigned int size, DiscoverFunc discover, ResolveFunc succeed, ResolveFunc fail)
		:active(size), discover_func(discover), succeed_func(succeed), fail_func(fail) {}

	DiscoverFunc discover_func;
	//Succeed if some new binding is active but not flesh.
	ResolveFunc succeed_func;
	//Fail if some new binding is flesh.
	ResolveFunc fail_func;
	BindingMap active;
};

struct ResourceConflictResolver {
	std::vector<ResolverClosure> group0, group1;

	void AddToGroup0(unsigned int size, ResolverClosure::DiscoverFunc discover, 
								ResolverClosure::ResolveFunc succeed, ResolverClosure::ResolveFunc fail);
	void AddToGroup1(unsigned int size, ResolverClosure::DiscoverFunc discover, 
								ResolverClosure::ResolveFunc succeed, ResolverClosure::ResolveFunc fail);
	void Resolve();
};

}






#endif			//RESOURCE_CONFLICT_RESOLVER_H_