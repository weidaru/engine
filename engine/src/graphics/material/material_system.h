#ifndef MATERIAL_SYSTEM_H_
#define MATERIAL_SYSTEM_H_

#include <vector>

namespace s2 {

class Material;

class MaterialSystem {
public:
	MaterialSystem();
	~MaterialSystem();

	void Register(Material *m);
	void Deregister(Material *m);

	void OneFrame(float delta);

private:
	std::vector<Material *> materials;
};


}

#endif // !MATERIAL_SYSTEM_H_
