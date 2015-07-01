#ifndef TEXT_SYSTEM_H_
#define TEXT_SYSTEM_H_

#include <vector>

#include "utils/s2string.h"
#include "entity/component_system.h"

struct IDWriteFactory;
struct IFW1FontWrapper;
struct IFW1Factory;

namespace s2 {
class Text;

class TextSystem : public ComponentSystem {
public:
	TextSystem();
	~TextSystem();

	virtual void Register(Component *c) override;
	virtual void Deregister(Component *c) override;

	static const s2string & GetDefaultFontName() {	
		static s2string default_name("Arial");
		return default_name;
	}

	void OneFrame(float delta);

private:
	IDWriteFactory * GetDWriteFactory() { return dwrite_factory; }

private:
	friend class Text;

private:
	std::vector<Text *> data;

	IFW1Factory *factory;
	IFW1FontWrapper *font_wrapper;
	IDWriteFactory *dwrite_factory;
};
}


#endif			//TEXT_SYSTEM_H_