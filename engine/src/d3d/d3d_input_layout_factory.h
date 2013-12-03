#ifndef	D3D_INPUT_LAYOUT_FACTORY_H_
#define D3D_INPUT_LAYOUT_FACTORY_H_

#include <string>
#include <map>

struct D3DVertexShader;
struct ID3D11InputLayout;
struct ID3D11Device;

class D3DInputLayoutFactory {
private:
	typedef std::map<std::string, ID3D11InputLayout *> Map;

public:
	D3DInputLayoutFactory(ID3D11Device *device);
	~D3DInputLayoutFactory();

	/*!
	  Bind the input index i to input slot j if input_slot_map[j] contains i,
	  If not i is not explicitly expressed to be bound with a particular slot, it is bounded to slot 0. 
	 */
	  ID3D11InputLayout  * Create(const std::string &name, D3DVertexShader *vs, const std::map<int, int> *input_slot_map);
	  ID3D11InputLayout * Get();

private:
	Map data_;
	ID3D11Device *device_;
};

#endif			//D3D_INPUT_LAYOUT_BUILDER_H_