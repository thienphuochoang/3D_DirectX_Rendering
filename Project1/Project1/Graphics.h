#pragma once
#include "CustomWin.h"
#include "CustomException.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
public:
    class Exception : public CustomException
    {
        using CustomException::CustomException;
    };
    class HrException : public Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {});
        const char* what() const override;
        const char* GetType() const override;
        HRESULT GetErrorCode() const;
        std::string GetErrorString() const;
        std::string GetErrorDescription() const;
        std::string GetErrorInfo() const;
    private:
        HRESULT hr;
        std::string info;
    };
    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;
    public:
        const char* GetType() const override;
    private:
        std::string reason;
    };
private:
    // Pointers to hold the necessary datas
    // Source: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
    ID3D11Device* pDevice = nullptr;
    IDXGISwapChain* pSwap = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* pTarget = nullptr;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics();
    void EndFrame();
    void ClearBuffer(float r, float g, float b);
};

