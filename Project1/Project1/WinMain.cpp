#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        Window wnd(1280, 720, "Cheap Engine");
        MSG msg;
        BOOL result;
        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (result == -1)
            return -1;
        // wParam is the value passed to the PostQuitMessage
        return msg.wParam;
    }
    catch (const CustomException& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}