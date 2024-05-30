#pragma once
#include "CustomWin.h"
#include <d3d11.h>

class Graphics
{
private:
    // Pointers to hold the necessary datas
    // Source: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float r, float g, float b);
};

