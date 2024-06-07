#include "App.h"
#include <sstream>
#include <iomanip>
#include "Box.h"
#include <memory>
App::App() : wnd(1280, 720, "Cheap Engine")
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
    std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
    std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
    std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
    for (auto i = 0; i < 10; i++)
    {
        boxes.push_back(std::make_unique<Box>(
            wnd.Gfx(), rng, adist,
            ddist, odist, rdist
        ));
    }
    wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

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
    //const float c = sin(timer.Peek()) / 2.0f + 0.5f;
    //wnd.Gfx().ClearBuffer(c, c, 1.0f);
    //wnd.Gfx().DrawTriangle(timer.Peek(), 
    //    wnd.mouseInput.GetPosX() / 640.0f - 1.0f, 
    //    -wnd.mouseInput.GetPosY() / 360.0f + 1.0f);
    //wnd.Gfx().DrawTriangle(-timer.Peek(),
    //    0,
    //    0);
    //wnd.Gfx().EndFrame();
    auto dt = timer.Mark();
    wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
    for (auto& b : boxes)
    {
        b->Update(dt);
        b->Draw(wnd.Gfx());
    }
    wnd.Gfx().EndFrame();
}
