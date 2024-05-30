#include "Window.h"
#include <sstream>
#include <iostream>
#include "resource.h"

Window::WindowClass Window::WindowClass::wndClass;
Window::WindowClass::WindowClass()
{
    hInstance = GetModuleHandle(nullptr);
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMessageSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

LPCWSTR Window::WindowClass::GetName() noexcept
{
    return L"Cheap Engine";
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInstance;
}

// Window Constructor
Window::Window(int inputWidth, int inputHeight, const char* name)
{
    // Calculate window size
    width = inputWidth;
    height = inputHeight;
    RECT rect;
    rect.left = 100;
    rect.right = width + rect.left;
    rect.top = 100;
    rect.bottom = height + rect.top;
    if (AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw LAST_CUSTOM_EXCEPTION();
    }
    // Create window and get hWnd
    hWnd = CreateWindowEx(0, WindowClass::GetName(), WindowClass::GetName(), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr,
        WindowClass::GetInstance(), this);

    // Check for error
    if (hWnd == nullptr)
    {
        throw LAST_CUSTOM_EXCEPTION();
    }
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
    DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& newTitle)
{
    if (SetWindowTextA(hWnd, newTitle.c_str()) == 0)
    {
        throw LAST_CUSTOM_EXCEPTION();
    }
}

std::optional<int> Window::ProcessMessages()
{
    MSG msg;
    // while queue has messages, remove and dispatch them
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        // Check for QuitMessage
        if (msg.message == WM_QUIT)
        {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return {};
}

Graphics& Window::Gfx()
{
    return *pGfx;
}

LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window instance
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
        // forward message to window instance handler
        return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCCREATE message, handle with default handler
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // retrieve pointer to window class
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window class handler
    return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        // Clear keystate when the user clicks on other applications, the window loses focus so it
        // doesn't get stuck on the keydown state
        case WM_KILLFOCUS:
        {
            keyboardInput.ClearState();
            break;
        }

        /* KEYBOARD INPUT MESSAGES */
        #pragma region KEYBOARD_INPUT_MESSAGES
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            // Filter auto repeat key events
            if (!(lParam & 0x40000000) || keyboardInput.IsAutoRepeatEnabled())
            {
                keyboardInput.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            keyboardInput.OnKeyReleased(static_cast<unsigned char>(wParam));
            break;
        }
        case WM_CHAR:
        {
            keyboardInput.OnChar(static_cast<unsigned char>(wParam));
            break;
        }
        #pragma endregion
        /* MOUSE INPUT MESSAGES */
        #pragma region MOUSE_INPUT_MESSAGES
        case WM_MOUSEMOVE:
        {
            POINTS point = MAKEPOINTS(lParam);
            // Check if the mouse position is in the window region, capture mouse position
            if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
            {
                mouseInput.OnMouseMove(point.x, point.y);
                if (!mouseInput.IsMouseInWindow())
                {
                    SetCapture(hWnd);
                    mouseInput.OnMouseEnter();
                }
            }
            // If the mouse position is not in the window region, still capture the mouse position
            // if the button is down
            else
            {
                if (wParam & (MK_LBUTTON | MK_RBUTTON))
                {
                    mouseInput.OnMouseMove(point.x, point.y);
                }
                // If the button is released
                else
                {
                    ReleaseCapture();
                    mouseInput.OnMouseLeave();
                }
            }
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouseInput.OnLeftMousePressed(point.x, point.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouseInput.OnRightMousePressed(point.x, point.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouseInput.OnLeftMouseReleased(point.x, point.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouseInput.OnRightMouseReleased(point.x, point.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS point = MAKEPOINTS(lParam);
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouseInput.OnMouseWheelDelta(point.x, point.y, delta);
            break;
        }
        #pragma endregion
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exception

Window::Exception::Exception(int line, const char* file, HRESULT hr) : 
    CustomException(line, file), hr(hr)
{}

const char* Window::Exception::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl;
    oss << "[Error Code] " << GetErrorCode() << std::endl;
    oss << "[Description] " << GetErrorString() << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const
{
    return "Custom Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
    char* pMsgBuff = nullptr;
    DWORD nMsgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuff), 0, nullptr);
    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }
    std::string errorString = pMsgBuff;
    LocalFree(pMsgBuff);
    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const
{
    return hr;
}

std::string Window::Exception::GetErrorString() const
{
    return TranslateErrorCode(hr);
}
