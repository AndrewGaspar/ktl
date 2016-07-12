#pragma once

#include <ktl\wdx.h>

class TestDeviceContext
{
public:
    TestDeviceContext()
    {

    }
};

WDX_DECLARE_CONTEXT_TYPE_WITH_NAME(TestDeviceContext, GetDeviceContext);

INIT NTSTATUS CreateTestDevice(_In_ WDFDRIVER driver);