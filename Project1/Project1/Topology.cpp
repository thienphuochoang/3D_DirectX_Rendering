#include "Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
{
    this->type = type;
}

void Topology::Bind(Graphics& gfx)
{
    GetContext(gfx)->IASetPrimitiveTopology(type);
}
