#include "GraphicsBinding.h"
ID3D11DeviceContext* GraphicsBinding::GetContext(Graphics& gfx)
{
	return gfx.pContext.Get();
}

ID3D11Device* GraphicsBinding::GetDevice(Graphics& gfx)
{
	return gfx.pDevice.Get();
}

DxgiInfoManager& GraphicsBinding::GetInfoManager(Graphics& gfx)
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("Can't get info manager");
#endif
}