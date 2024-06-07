#pragma once
#include "Window.h"
#include "CustomTimer.h"
#include "Box.h"
class App
{
public:
    App();
    int Begin();
private:
    Window wnd;
    CustomTimer timer;
    void Update();
    std::vector<std::unique_ptr<Box>> boxes;
};

