#pragma once
#include "CustomWin.h"
#include "CustomException.h"
#include "KeyboardInput.h"
#include "MouseInput.h"
#include "Graphics.h"
#include "CustomMacros.h"
#include <optional>
#include <memory>

class Window
{
public:
    class Exception : public CustomException
    {
        using CustomException::CustomException;
    public:
        static std::string TranslateErrorCode(HRESULT hr);
    };
    class HrException : public Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr);
        const char* what() const override;
        const char* GetType() const override;
        HRESULT GetErrorCode() const;
        std::string GetErrorDescription() const;
    private:
        HRESULT hr;
    };
    class NoGfxException : public Exception
    {
    public:
        using Exception::Exception;
        const char* GetType() const override;
    };
private:
    // Singleton manages registration/cleanup of window class
    class WindowClass
    {
    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        LPCWSTR wndClassName = L"Real Engine";
        static WindowClass wndClass;
        HINSTANCE hInstance;
    public:
        static LPCWSTR GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    };
public:
    Window(int width, int height, const char* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    KeyboardInput keyboardInput;
    MouseInput mouseInput;
    void SetTitle(const std::string& newTitle);
    static std::optional<int> ProcessMessages();
    Graphics& Gfx();
private:
    int width;
    int height;
    HWND hWnd;
    std::unique_ptr<Graphics> pGfx;
    static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};