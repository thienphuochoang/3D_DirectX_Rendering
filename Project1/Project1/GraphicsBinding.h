#pragma once
#include "Graphics.h"
class GraphicsBinding
{
public:
    virtual ~GraphicsBinding() = default;
    virtual void Bind(Graphics& gfx) = 0;
protected:
    static ID3D11DeviceContext* GetContext(Graphics& gfx);
    static ID3D11Device* GetDevice(Graphics& gfx);
    static DxgiInfoManager& GetInfoManager(Graphics& gfx);
};