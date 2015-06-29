#ifndef SCENE_H_
#define SCENE_H_

#include "utils/s2string.h"
#include <map>

namespace s2  {
class Entity;

/**
 * Container of entities read from file.
 */
class Scene {
public:
	Scene();
	~Scene();

	bool Initialize(const s2string &path);
	s2string GetLastError() { return error; }

private:
	s2string error;
	std::map<s2string, Entity *> entities;
};

}

#endif // !SCENE_H_
