#pragma once
#include "GraphicsBinding.h"

class IndexBuffer : public GraphicsBinding
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<short>& indices);
	void Bind(Graphics& gfx) override;
	UINT GetCount() const;
protected:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};
