#ifndef D3D_VIEWPORT_FACTORY_H_
#define D3D_VIEWPORT_FACTORY_H_

#include <map>

struct D3D11_VIEWPORT;

class D3DViewportFactory {
private:
	typedef std::map<std::string, D3D11_VIEWPORT *> Map;

public:
	D3DViewportFactory();
	~D3DViewportFactory();

	D3D11_VIEWPORT * GetDefault();
	D3D11_VIEWPORT * Get(const std::string &name);
	D3D11_VIEWPORT * Create(const std::string &name, float topleft_x, float topleft_y, float width, float height);
	void Remove(const std::string &name);

private:
	Map data_;
};

#endif			//D3D_VIEWPORT_FACTORY_H_