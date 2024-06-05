#include "App.h"
#include <sstream>
#include <iomanip>
App::App() : wnd(1280, 720, "Cheap Engine")
{}

int App::Begin()
{
    while (true)
    {
        // Process all messages pending
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        Update();
    }
}

void App::Update()
{
    const float c = sin(timer.Peek()) / 2.0f + 0.5f;
    wnd.Gfx().ClearBuffer(c, c, 1.0f);
    wnd.Gfx().DrawTriangle();
    wnd.Gfx().EndFrame();
}
