#ifndef MATERIAL_SYSTEM_H_
#define MATERIAL_SYSTEM_H_

#include <vector>

#include "entity/component_system.h"

namespace s2 {

class Material;

class MaterialSystem : public ComponentSystem  {
public:
	MaterialSystem();
	virtual ~MaterialSystem() override;

	virtual void Register(Component *c) override;
	virtual void Deregister(Component *c) override;

	void OneFrame(float delta);

private:
	std::vector<Material *> materials;
};


}

#endif // !MATERIAL_SYSTEM_H_
