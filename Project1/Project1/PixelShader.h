#pragma once
#include "GraphicsBinding.h"
#include <d3dcompiler.h>
class PixelShader : public GraphicsBinding
{
public:
	PixelShader(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) override;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};

