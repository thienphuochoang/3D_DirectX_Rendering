#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include "CustomMacros.h"
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace dx = DirectX;

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

    UINT swapCreateFlags = 0;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // a local variable for checking result of d3d functions (GFX_THROW_FAILED macro above)
    HRESULT hr;

    // Create device, swap chain, rendering context and front/back buffers
    // Source: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext
    ));
    // Gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
    
    // Create depth stencil state
    D3D11_DEPTH_STENCIL_DESC deDesc = {};
    deDesc.DepthEnable = TRUE;
    deDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    deDesc.DepthFunc = D3D11_COMPARISON_LESS;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
    GFX_THROW_INFO(pDevice->CreateDepthStencilState(&deDesc, &pDSState));

    // bind depth state
    pContext->OMSetDepthStencilState(pDSState.Get(), 1);

    // create depth stencil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = 1280;
    descDepth.Height = 720;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

    // Create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDepthStencilView));

    // Bind Depth Stencil View to OM
    pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDepthStencilView.Get());

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = 1280.0f;
    vp.Height = 720.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    pContext->RSSetViewports(1u, &vp);
}

//Graphics::~Graphics()
//{
//    if (pContext != nullptr)
//        pContext->Release();
//    if (pSwap != nullptr)
//        pSwap->Release();
//    if (pDevice != nullptr)
//        pDevice->Release();
//    if (pTarget != nullptr)
//        pTarget->Release();
//}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifndef NDEBUG
    infoManager.Set();
#endif
    if (FAILED(hr = pSwap->Present(1, 0)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw GFX_EXCEPT(hr);
        }
    }
    
}

void Graphics::ClearBuffer(float r, float g, float b)
{
    const float color[] = { r,g,b,1.0f };
    pContext->ClearRenderTargetView(pTarget.Get(), color);
    pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::DrawTriangle(float angle, float x, float z)
{
    // Define a smart pointer type for COM objects
    using Microsoft::WRL::ComPtr;

    // Define a structure for a vertex with 2D position coordinates
    struct Vertex
    {
        struct
        {
            float x;
            float y;
            float z;
        } pos;
    };

    // Define an array of vertices representing a triangle
    Vertex vertices[] =
    {
        { -1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { -1.0f, 1.0f, -1.0f},
        { 1.0f, 1.0f, -1.0f},
        { -1.0f, -1.0f, 1.0f},
        { 1.0f, -1.0f, 1.0f},
        { -1.0f, 1.0f, 1.0f},
        { 1.0f, 1.0f, 1.0f}
    };

    // Define a smart pointer to a Direct3D vertex buffer
    ComPtr<ID3D11Buffer> pVertexBuffer;

    // Describe the vertex buffer
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;  // Use this buffer as a vertex buffer
    bd.Usage = D3D11_USAGE_DEFAULT;           // Default usage
    bd.CPUAccessFlags = 0;                    // No CPU access needed
    bd.MiscFlags = 0;                         // No miscellaneous flags
    bd.ByteWidth = sizeof(vertices);          // Total size of the buffer
    bd.StructureByteStride = sizeof(Vertex);  // Size of one vertex

    // Provide the vertex data for the buffer
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;  // Pointer to the vertex data

    // Create the vertex buffer on the GPU
    HRESULT hr;
    GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

    // Define the stride (size of each vertex) and the offset (start of the buffer)
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;

    // Bind the vertex buffer to the input assembler stage
    pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);

    // Create index buffer
    const short indices[] =
    {
        0,2,1,  2,3,1,
        1,3,5,  3,7,5,
        2,6,3,  3,6,7,
        4,5,7,  4,7,6,
        0,4,2,  2,4,6,
        0,1,4,  1,5,4
    };

    ComPtr<ID3D11Buffer> pIndexBuffer;
    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));

    // Bind index buffer
    pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixRotationZ(angle) *
                dx::XMMatrixRotationX(angle) *
                dx::XMMatrixTranslation(x, 0.0f, z + 4.0f) *
                dx::XMMatrixPerspectiveLH(1.0f, 720.0f / 1280.0f, 0.5f, 10.0f))
        }
    };
    ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // Bind constant buffer to vertex shader
    pContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

    struct ConstantBuffer2
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } face_colors[6];
    };
    const ConstantBuffer2 cb2 =
    {
        {
            {1.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 1.0f}
        }
    };
    ComPtr<ID3D11Buffer> pConstantBuffer2;
    D3D11_BUFFER_DESC cbd2;
    cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd2.Usage = D3D11_USAGE_DEFAULT;
    cbd2.CPUAccessFlags = 0;
    cbd2.MiscFlags = 0;
    cbd2.ByteWidth = sizeof(cb2);
    cbd2.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA csd2 = {};
    csd2.pSysMem = &cb2;
    GFX_THROW_INFO(pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

    // Bind constant buffer to pixel shader
    pContext->PSSetConstantBuffers(0, 1, pConstantBuffer2.GetAddressOf());

    // Define smart pointers for the pixel shader and blob
    ComPtr<ID3D11PixelShader> pPixelShader;
    ComPtr<ID3DBlob> pBlob;

    // Load and create the pixel shader
    GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

    // Bind the pixel shader to the pipeline
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

    // Define a smart pointer for the vertex shader
    ComPtr<ID3D11VertexShader> pVertexShader;

    // Load and create the vertex shader
    GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

    // Bind the vertex shader to the pipeline
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

    // Define the input layout for the vertex data (2D position)
    ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create the input layout object
    GFX_THROW_INFO(pDevice->CreateInputLayout(
        ied, (UINT)std::size(ied),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout
    ));

    // Bind the input layout to the pipeline
    pContext->IASetInputLayout(pInputLayout.Get());

    // Set the primitive topology to triangles
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Define and set the viewport
    D3D11_VIEWPORT vp;
    vp.Width = 1280;   // Width of the viewport
    vp.Height = 720;   // Height of the viewport
    vp.MinDepth = 0;   // Minimum depth of the viewport
    vp.MaxDepth = 1;   // Maximum depth of the viewport
    vp.TopLeftX = 0;   // Top-left X coordinate
    vp.TopLeftY = 0;   // Top-left Y coordinate
    pContext->RSSetViewports(1, &vp);
}

void Graphics::DrawIndexed(UINT count)
{
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0, 0));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj)
{
    projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const
{
    return projection;
}

// Graphics Exception

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) :
    Exception(line, file), hr(hr)
{
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl;
    oss << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode();
    oss << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl;
    oss << "[Error String] " << GetErrorString() << std::endl;
    oss << "[Error Description] " << GetErrorDescription() << std::endl;
    if (!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const
{
    return "Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const
{
    return hr;
}

std::string Graphics::HrException::GetErrorString() const
{
    return DXGetErrorStringA(hr);
}

std::string Graphics::HrException::GetErrorDescription() const
{
    char buff[512];
    DXGetErrorDescriptionA(hr, buff, sizeof(buff));
    return buff;
}

std::string Graphics::HrException::GetErrorInfo() const
{
    return info;
}

const char* Graphics::DeviceRemovedException::GetType() const
{
    return "Graphics Exception [Device Removed]";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) :
    Exception(line, file)
{
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::InfoException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl;
    oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const
{
    return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const
{
    return info;
}
