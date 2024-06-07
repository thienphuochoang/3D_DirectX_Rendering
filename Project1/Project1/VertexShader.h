#pragma once
#include "GraphicsBinding.h"
#include <d3dcompiler.h>
class VertexShader : public GraphicsBinding
{
public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) override;
	ID3DBlob* GetBytecode() const;
protected:
	Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};

