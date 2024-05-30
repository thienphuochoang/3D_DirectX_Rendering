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
    const float t = timer.Peek();
    std::ostringstream oss;
    oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
    wnd.SetTitle(oss.str());
}
