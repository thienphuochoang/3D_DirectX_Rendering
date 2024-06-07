#pragma once
#include "CustomWin.h"
#include "CustomException.h"
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include "DxgiInfoManager.h"
#include <DirectXMath.h>

class Graphics
{
    friend class GraphicsBinding;
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
    class InfoException : public Exception
    {
    private:
        std::string info;
    public:
        InfoException(int line, const char* file, std::vector<std::string> infoMsgs = {});
        const char* what() const override;
        const char* GetType() const override;
        std::string GetErrorInfo() const;
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
    DirectX::XMMATRIX projection;
    // Pointers to hold the necessary datas
    // Source: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
#ifndef NDEBUG
    DxgiInfoManager infoManager;
#endif
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    ~Graphics() = default;
    void EndFrame();
    void ClearBuffer(float r, float g, float b);
    void DrawTriangle(float angle, float x, float y);
    void DrawIndexed(UINT count);
    void SetProjection(DirectX::FXMMATRIX proj);
    DirectX::XMMATRIX GetProjection() const;
};

