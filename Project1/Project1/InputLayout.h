#pragma once
#include "GraphicsBinding.h"

class InputLayout : public GraphicsBinding
{
public:
	InputLayout(Graphics& gfx,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* pVertexShaderBytecode);
	void Bind(Graphics& gfx) override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};

