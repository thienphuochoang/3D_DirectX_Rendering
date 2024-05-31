#pragma once
#include "CustomWin.h"
#include <vector>
#include <string>
class DxgiInfoManager
{
private:
    unsigned long long next = 0;
    struct IDXGIInfoQueue* pDxgiInfoQueue = nullptr;
public:
    DxgiInfoManager();
    ~DxgiInfoManager();
    DxgiInfoManager(const DxgiInfoManager&) = delete;
    DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
    void Set();
    std::vector<std::string> GetMessages() const;
};

