#include "TransformConstantBuffer.h"
TransformConstantBuffer::TransformConstantBuffer(Graphics& gfx, const GraphicsDrawing& parent)
	:
	parent(parent)
{
	if (!pvcBuff)
	{
		pvcBuff = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformConstantBuffer::Bind(Graphics& gfx)
{
	pvcBuff->Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection()));
	pvcBuff->Bind(gfx);
}
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformConstantBuffer::pvcBuff;