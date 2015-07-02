#ifndef SCENE_H_
#define SCENE_H_

#include "utils/s2string.h"
#include <map>

namespace Assimp {
class Importer;
}

struct aiNode;
struct aiScene;

namespace s2  {
class Entity;

/**
 * Container of entities read from file.
 * These entities are concerned as local to the scene.
 */
class Scene {
public:
	Scene();
	~Scene();

	bool Initialize(const s2string &path);
	s2string GetLastError() { return error; }

private:
	bool ProcessNode(aiNode *node, const aiScene *scene);

private:
	s2string error;
	std::map<s2string, Entity *> entities;

	Assimp::Importer *importer;
};

}

#endif // !SCENE_H_
