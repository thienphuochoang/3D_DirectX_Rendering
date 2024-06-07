#include "GraphicsDrawing.h"
#include "CustomMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void GraphicsDrawing::Draw(Graphics& gfx) const
{
    for (auto& bind : binds)
    {
        bind->Bind(gfx);
    }
    for (auto& bind : GetStaticBinds())
    {
        bind->Bind(gfx);
    }
    gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void GraphicsDrawing::AddBind(std::unique_ptr<GraphicsBinding> bind)
{
    assert("MUST use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
    binds.push_back(std::move(bind));
}

void GraphicsDrawing::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuffer)
{
    assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
    pIndexBuffer = ibuffer.get();
    binds.push_back(std::move(ibuffer));
}


