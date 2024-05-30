#pragma once
#include "Window.h"
#include "CustomTimer.h"
class App
{
public:
    App();
    int Begin();
private:
    Window wnd;
    CustomTimer timer;
    void Update();
};

