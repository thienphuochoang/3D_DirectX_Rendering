#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
class GraphicsBinding;
class GraphicsDrawing
{
    template<class T>
    friend class GraphicsDrawingBase;
public:
    GraphicsDrawing() = default;
    GraphicsDrawing(const GraphicsDrawing&) = delete;
    virtual ~GraphicsDrawing() = default;
    virtual DirectX::XMMATRIX GetTransformXM() const = 0;
    void Draw(Graphics& gfx) const;
    virtual void Update(float dt) = 0;
    void AddBind(std::unique_ptr<GraphicsBinding> bind);
    void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuffer);
private:
    const class IndexBuffer* pIndexBuffer = nullptr;
    std::vector<std::unique_ptr<GraphicsBinding>> binds;
    virtual const std::vector<std::unique_ptr<GraphicsBinding>>& GetStaticBinds() const = 0;
};

