#include "Graphics.h"
#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;
    // Create device, swap chain, rendering context and front/back buffers
    // Source: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    );
    // Gain access to texture subresource in swap chain (back buffer)
    ID3D11Resource* pBackBuffer = nullptr;
    pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    pBackBuffer->Release();
}

Graphics::~Graphics()
{
    if (pContext != nullptr)
        pContext->Release();
    if (pSwap != nullptr)
        pSwap->Release();
    if (pDevice != nullptr)
        pDevice->Release();
    if (pTarget != nullptr)
        pTarget->Release();
}

void Graphics::EndFrame()
{
    pSwap->Present(1, 0);
}

void Graphics::ClearBuffer(float r, float g, float b)
{
    const float color[] = { r,g,b,1.0f };
    pContext->ClearRenderTargetView(pTarget, color);
}
