#include "Precomp.h"

// to include single implementation of default operator new and delete
#include "newdef.cpp"

#include "Tracing.h"
#include "Driver.h"
#include "Device.h"

#include <ktl\scope.h>
#include <ktl\wdx.h>

#include <ntimage.h>

// {EB9CC4E7-A09F-49E3-93F6-D20C6FC89FF5}
TRACELOGGING_DEFINE_PROVIDER(KtlTestTracing, "KtlTestTracing", 
    (0xeb9cc4e7, 0xa09f, 0x49e3, 0x93, 0xf6, 0xd2, 0xc, 0x6f, 0xc8, 0x9f, 0xf5));

EXTERN_C INIT NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registryPath)
{
    auto registration = TraceLoggingRegistration(KtlTestTracing);

    LogEntry();

    WDF_OBJECT_ATTRIBUTES driverAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&driverAttributes);
    WDX_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&driverAttributes, DriverContext);

    WDF_DRIVER_CONFIG driverConfig;
    WDF_DRIVER_CONFIG_INIT(&driverConfig, nullptr);
    driverConfig.DriverInitFlags = WdfDriverInitNonPnpDriver;

    driverConfig.EvtDriverUnload = [](WDFDRIVER) PAGED {
        // do nothing, just to allow driver to be stopped
        // all cleanup is done via DriverContext's destructor
    };

    WDFDRIVER wdfDriver;
    RETURN_IF_NOT_NT_SUCCESS(WdfDriverCreate(driver, registryPath, &driverAttributes, &driverConfig, &wdfDriver));

    WDX_INIT_CONTEXT(wdfDriver, DriverContext, std::move(registration));

    RETURN_IF_NOT_NT_SUCCESS(CreateTestDevice(wdfDriver));

    SuccessExit();
}