#include "resource_conflict_resolver.h"

#include "renderer/resource.h"

#include <glog/logging.h>

namespace s2 {

namespace {

bool Contains(const BindingVec &vec, Resource *resource) {
	for (unsigned int i = 0; i<vec.size(); i++) {
		if (vec[i].second == resource) {
			return true;
		}
	}
	return false;
}

}


BindingMap::BindingMap(unsigned int size) {
	vec.resize(size, 0);
}

bool BindingMap::Contains(unsigned int index) {
	return vec[index] == 0;
}

bool BindingMap::Contains(Resource *resource) {
	return map.find(resource) != map.end();
}

BindingMap & BindingMap::Remove(unsigned int index) {
	if (Contains(index)) {
		map.erase(vec[index]);
		vec[index] = 0;
	}
	return *this;
}

BindingMap & BindingMap::Remove(Resource *resource) {
	if (Contains(resource)) {
		vec[map[resource]] = 0;
		map.erase(resource);
	}
	return *this;
}

BindingMap & BindingMap::Add(unsigned int index, Resource *resource) {
	CHECK(resource) << "Resource should not be 0";
	vec[index] = resource;
	map[resource] = index;
	return *this;
}

Resource * BindingMap::GetResource(unsigned int index) {
	return vec[index];
}

unsigned int BindingMap::GetIndex(Resource *resource) {
	CHECK(Contains(resource)) << "Resource is not in map";
	return map[resource];
}

void BindingMap::Update(const std::vector<std::pair<unsigned int, Resource *> > &new_things) {
	for (unsigned int j = 0; j < new_things.size(); j++) {
		Resource *rt = new_things[j].second;
		unsigned int index = new_things[j].first;
		if (rt) {
			Add(index, rt);
		}
		else {
			Remove(index);
		}
	}
}


void ResourceConflictResolver::AddToGroup0(unsigned int size, ResolverClosure::DiscoverFunc discover, 
									ResolverClosure::ResolveFunc succeed, ResolverClosure::ResolveFunc fail) {
	group0.push_back(ResolverClosure(size, discover, succeed, fail));
}

void ResourceConflictResolver::AddToGroup1(unsigned int size, ResolverClosure::DiscoverFunc discover, 
									ResolverClosure::ResolveFunc succeed, ResolverClosure::ResolveFunc fail) {
	group1.push_back(ResolverClosure(size, discover, succeed, fail));
}


namespace {

void ResolveInternal(std::vector<ResolverClosure> &lhs, std::vector<BindingVec *> &lhs_new,
					std::vector<ResolverClosure> &rhs, std::vector<BindingVec *> &rhs_new) {
	for (unsigned int i = 0; i < lhs.size(); i++) {
		BindingVec &candicate = *lhs_new[i];
		for (unsigned int j = 0; j < rhs.size(); j++) {
			ResolverClosure &closure = rhs[j];
			BindingVec &new_binding = *rhs_new[j];
			for (unsigned int k = 0; k < candicate.size(); k++) {
				Resource *res = candicate[k].second;
				if (closure.active.Contains(res) && Contains(new_binding, res) == false) {
					closure.succeed_func(closure.active.GetIndex(res), res);
				}
				else if (Contains(new_binding, res)) {
					closure.fail_func(closure.active.GetIndex(res), res);
				}
			}
		}
	}
}

void GetNewThings(const std::vector<ResolverClosure> &group, std::vector<BindingVec *> *result) {
	for (unsigned int i = 0; i < group.size(); i++) {
		result->push_back(new BindingVec(group[i].discover_func()));
	}
}

void Update(std::vector<ResolverClosure> *_group) {
	std::vector<ResolverClosure> &group = *_group;
	for (unsigned int i = 0; i < group.size(); i++) {
		 BindingVec new_things = group[i].discover_func();
		 group[i].active.Update(new_things);
	}
}

}

void ResourceConflictResolver::Resolve() {
	std::vector<BindingVec *> group0_new;
	GetNewThings(group0, &group0_new);
	std::vector<BindingVec *> group1_new;
	GetNewThings(group1, &group1_new);
		
	ResolveInternal(group0, group0_new, group1, group1_new);
	ResolveInternal(group1, group1_new, group0, group0_new);

	Update(&group0);
	Update(&group1);

	for (auto item : group1_new) {
		delete item;
	}
	for (auto item : group0_new) {
		delete item;
	}
}


}

