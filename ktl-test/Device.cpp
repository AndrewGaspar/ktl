#include "Precomp.h"

#include "Device.h"
#include "Driver.h"
#include "Tracing.h"
#include "UnitTest.h"

#include <numeric>
#include <algorithm>
#include <KtlTestPublic.h>
#include <ktl\string.h>

PAGED PASSIVE EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtTestDeviceIoctl;

_Use_decl_annotations_
INIT NTSTATUS CreateTestDevice(
    _In_ WDFDRIVER Driver)
{
    LogEntry();

    auto deviceInit = WdfControlDeviceInitAllocate(Driver, &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_R);

    constexpr auto UnicodeKtlTestDeviceName = ktl::make_const_unicode_string(KtlTestDeviceName);

    RETURN_IF_NOT_NT_SUCCESS(WdfDeviceInitAssignName(deviceInit, &UnicodeKtlTestDeviceName));
    WdfDeviceInitSetIoType(deviceInit, WdfDeviceIoBuffered);
    WdfDeviceInitSetDeviceType(deviceInit, FILE_DEVICE_UNKNOWN);
    
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDX_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, TestDeviceContext);

    deviceAttributes.ExecutionLevel = WdfExecutionLevelPassive;

    WDFDEVICE device;
    RETURN_IF_NOT_NT_SUCCESS(WdfDeviceCreate(&deviceInit, &deviceAttributes, &device));
    WDX_INIT_CONTEXT(device, TestDeviceContext);

    constexpr auto UnicodeKtlTestDeviceSymLink = ktl::make_const_unicode_string(KtlTestDeviceSymlink);
    RETURN_IF_NOT_NT_SUCCESS(WdfDeviceCreateSymbolicLink(device, &UnicodeKtlTestDeviceSymLink));

    WDF_IO_QUEUE_CONFIG ioQueueConfig;
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioQueueConfig, WdfIoQueueDispatchParallel);

    ioQueueConfig.EvtIoDeviceControl = EvtTestDeviceIoctl;

    WDFQUEUE queue;
    RETURN_IF_NOT_NT_SUCCESS(WdfIoQueueCreate(device, &ioQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue));

    SuccessExit();
}

PAGED void HandleListTestsRequest(_In_ WDFREQUEST Request)
{
    LogEntry();

    // default size of 1 for end null-terminator
    auto size = std::accumulate(TestBegin(), TestEnd(), std::size_t(1),
        [](size_t const & total, TestMetadata const & test) {
        return total + wcslen(test.Name) + 1;
    }) * sizeof(wchar_t);

    wchar_t * buffer = nullptr;
    {
        auto status = WdfRequestRetrieveOutputBuffer(Request, size, (PVOID *)&buffer, nullptr);
        if (status == STATUS_BUFFER_TOO_SMALL)
        {
            WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_OVERFLOW, size);
            return;
        }
        else if (!NT_SUCCESS(status))
        {
            WdfRequestComplete(Request, status);
            return;
        }
    }

    for (auto test = TestBegin(); test != TestEnd(); test++)
    {
        // copy through null terminator
        buffer = std::copy(test->Name, test->Name + wcslen(test->Name) + 1, buffer);
    }
    *buffer = L'\0';

    WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, size);

    LogExit();
}

PAGED void HandleExecTestRequest(_In_ WDFREQUEST Request)
{
    LogEntry();

    auto result = [&]() {

        PVOID buffer;
        std::size_t bufferSize;
        RETURN_IF_NOT_NT_SUCCESS(WdfRequestRetrieveInputBuffer(Request, sizeof(wchar_t), &buffer, &bufferSize));

        if (bufferSize % sizeof(wchar_t) != 0)
        {
            RETURN_IF_NOT_NT_SUCCESS(STATUS_INVALID_BUFFER_SIZE);
        }

        auto str = static_cast<PCWSTR>(buffer);
        auto strSize = bufferSize / sizeof(wchar_t);
        auto bufEnd = str + strSize;

        auto strEnd = std::find(str, bufEnd, L'\0');
        if (strEnd == bufEnd) RETURN_IF_NOT_NT_SUCCESS(STATUS_INVALID_PARAMETER);

        auto const testName = str;
        auto test = std::find_if(TestBegin(), TestEnd(), [=](TestMetadata const & metaData) {
            return 0 == _wcsicmp(metaData.Name, testName);
        });

        if (test == TestEnd()) RETURN_IF_NOT_NT_SUCCESS(STATUS_INVALID_PARAMETER);

        return test->Test();
    }();

    WdfRequestComplete(Request, result);

    LogExit();
}

_Use_decl_annotations_
PAGED void EvtTestDeviceIoctl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode)
{
    LogEntry();

    UNREFERENCED_PARAMETER((Queue, OutputBufferLength, InputBufferLength));

    switch ((KtlTestIoctl)IoControlCode)
    {
    case KtlTestIoctl::ListTests:
        HandleListTestsRequest(Request);
        break;
    case KtlTestIoctl::ExecTest:
        HandleExecTestRequest(Request);
        break;
    default:
        WdfRequestComplete(Request, STATUS_NOT_IMPLEMENTED);
        break;
    }

    LogExit();
}
