#pragma once
#include <chrono>
class CustomTimer
{
private:
    std::chrono::steady_clock::time_point last;
public:
    CustomTimer();
    float Mark();
    float Peek() const;
};

