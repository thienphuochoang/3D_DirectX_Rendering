#pragma once
#include "GraphicsBinding.h"
class Topology : public GraphicsBinding
{
public:
    Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
    void Bind(Graphics& gfx) override;
protected:
    D3D11_PRIMITIVE_TOPOLOGY type;
};

