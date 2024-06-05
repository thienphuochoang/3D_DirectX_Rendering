#pragma once
#include "CustomWin.h"
#include <vector>
#include <string>
#include <wrl.h>
#include <dxgidebug.h>
class DxgiInfoManager
{
private:
    unsigned long long next = 0;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
public:
    DxgiInfoManager();
    ~DxgiInfoManager() = default;
    DxgiInfoManager(const DxgiInfoManager&) = delete;
    DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
    void Set();
    std::vector<std::string> GetMessages() const;
};

