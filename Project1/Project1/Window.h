#pragma once
#include "CustomWin.h"
#include "CustomException.h"
#include "KeyboardInput.h"
#include "MouseInput.h"
#include <optional>

class Window
{
public:
    class Exception : public CustomException
    {
    public:
        Exception(int line, const char* file, HRESULT hr);
        const char* what() const override;
        const char* GetType() const override;
        static std::string TranslateErrorCode(HRESULT hr);
        HRESULT GetErrorCode() const;
        std::string GetErrorString() const;
    private:
        HRESULT hr;
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
private:
    int width;
    int height;
    HWND hWnd;
    static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// error exception macro
#define CUSTOM_EXCEPTION(hr) Window::Exception(__LINE__, __FILE__, hr)
#define LAST_CUSTOM_EXCEPTION() Window::Exception(__LINE__, __FILE__, GetLastError());