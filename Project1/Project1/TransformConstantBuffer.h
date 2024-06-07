#pragma once
#include "ConstantBuffer.h"
#include "GraphicsDrawing.h"
#include <DirectXMath.h>

class TransformConstantBuffer : public GraphicsBinding
{
public:
	TransformConstantBuffer(Graphics& gfx, const GraphicsDrawing& parent);
	void Bind(Graphics& gfx) override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> pvcBuff;
	const GraphicsDrawing& parent;
};
