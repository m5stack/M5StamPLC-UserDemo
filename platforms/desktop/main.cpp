/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <app.h>
#include "hal_desktop/hal_desktop.hpp"
#include <shared/shared.h>
#include <thread>
#include <mutex>

class MySharedData : public SharedData {
public:
    std::mutex _mutex;

    SHARED_DATA::SharedData_t& borrowData() override
    {
        _mutex.lock();
        return _data;
    }

    void returnData() override
    {
        _mutex.unlock();
    }
};

void setup()
{
    APP::SetupCallback_t callback;

    callback.AssetPoolInjection = []() {
        AssetPool::CreateStaticAssetBin(AssetPool::CreateStaticAsset());
        AssetPool::InjectStaticAsset(AssetPool::GetStaticAssetFromBin());
    };

    callback.HalInjection = []() { HAL::Inject(new HAL_Desktop(240, 135)); };

    callback.sharedDataInjection = []() { SharedData::Inject(new MySharedData); };

    APP::Setup(callback);
}

void loop()
{
    APP::Loop();
}
