#pragma once
#include <unordered_map>
#include <Windows.h>
#include <string>
using namespace std;
class WindowMessageMap
{
public:
    WindowMessageMap();
    std::string PrintWindowMessage(DWORD msg) const;
private:
    unordered_map<DWORD, string> map;
};