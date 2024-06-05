#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
    //pBackBuffer->Release();
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
}

void Graphics::DrawTriangle()
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
        } pos;
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } color;
    };

    // Define an array of vertices representing a triangle
    Vertex vertices[] =
    {
        { 0.0f, 0.5f, 255, 0, 0, 0 },
        { 0.5f, -0.5f, 0, 255, 0, 0 },
        { -0.5f, -0.5f, 0, 0, 255, 0 },
        { -0.3f, 0.3f, 0, 0, 255, 0 },
        { 0.3f, 0.3f, 0, 0, 255, 0 },
        { 0.0f, -0.8f, 0, 0, 255, 0 }
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
        0,1,2,
        0,2,3,
        0,4,1,
        2,1,5
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
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

    // Bind the render target view to the output merger stage
    pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);

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

    // Draw the triangle
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0, 0));
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
