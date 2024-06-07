#include "VertexBuffer.h"

void VertexBuffer::Bind(Graphics& gfx)
{
	const UINT offset = 0;
	GetContext(gfx)->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
}