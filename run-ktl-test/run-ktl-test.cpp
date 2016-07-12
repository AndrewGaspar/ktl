// run-ktl-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <KtlTestPublic.h>
#include <wtl\service.h>

using namespace wtl;

auto get_test_device() -> hresult_t<handle>
{
    handle file = CreateFileW(
        KtlTestDevicePath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (!file) return HRESULT_FROM_WIN32(GetLastError());

    return hresult_t<handle>::success(std::move(file));
}

auto list_tests() -> hresult
{
    RETURN_OR_UNWRAP(file, get_test_device());

    ULONG requiredSize = 0;
    if (!DeviceIoControl(
        file.get(),
        (ULONG)KtlTestIoctl::ListTests,
        nullptr,
        0,
        nullptr,
        0,
        &requiredSize,
        nullptr))
    {
        if (GetLastError() != ERROR_MORE_DATA)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }
    }

    std::vector<wchar_t> files(requiredSize / sizeof(wchar_t) + requiredSize % sizeof(wchar_t));
    if (!DeviceIoControl(
        file.get(),
        (ULONG)KtlTestIoctl::ListTests,
        nullptr,
        0,
        files.data(),
        (ULONG)(files.size() * sizeof(wchar_t)),
        &requiredSize,
        nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    auto it = files.begin();
    while (*it != L'\0')
    {
        std::wcout << &*it << std::endl;
        it += wcslen(&*it) + 1;
    }

    return S_OK;
}

auto exec_test(PCWSTR testName) -> hresult
{
    RETURN_OR_UNWRAP(file, get_test_device());

    if (!DeviceIoControl(
        file.get(),
        static_cast<DWORD>(KtlTestIoctl::ExecTest),
        (PVOID)testName,
        (wcslen(testName) + 1) * sizeof(wchar_t),
        nullptr,
        0,
        nullptr,
        nullptr))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

auto delete_service() -> hresult
{
    sc_handle scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scm) return HRESULT_FROM_WIN32(GetLastError());

    sc_handle service = OpenServiceW(scm.get(), L"ktl-test", SERVICE_ALL_ACCESS);
    if (!service) return HRESULT_FROM_WIN32(GetLastError());

    if (!DeleteService(service.get())) return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

auto register_service() -> hresult
{
    sc_handle scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scm) return HRESULT_FROM_WIN32(GetLastError());

    sc_handle service = CreateServiceW(
        scm.get(),
        L"ktl-test",
        L"KTLTest Service",
        SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER,
        SERVICE_AUTO_START,
        SERVICE_ERROR_IGNORE,
        L"C:\\Windows\\System32\\ktl-test.sys",
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr);
    if (!service) return HRESULT_FROM_WIN32(GetLastError());

    if (!StartServiceW(service.get(), 0, nullptr)) return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}

auto hr_main(PCWSTR exe, PWSTR * begin, PWSTR * end) -> hresult
{
    if (begin == end)
    {
        std::wcerr << L"Must specify <command>" << std::endl;
        return E_INVALIDARG;
    }

    if (0 == _wcsicmp(*begin, L"register"))
    {
        return register_service();
    }

    if (0 == _wcsicmp(*begin, L"unregister"))
    {
        return delete_service();
    }
    
    if (0 == _wcsicmp(*begin, L"list"))
    {
        return list_tests();
    }

    if (0 == _wcsicmp(*begin, L"exec"))
    {
        if (begin + 1 == end)
        {
            std::wcerr << L"Must provide test name" << std::endl;
            return E_INVALIDARG;
        }

        return exec_test(begin[1]);
    }

    return S_OK;
}

int wmain(int argc, wchar_t **argv)
{
    auto result = hr_main(*argv, argv + 1, argv + argc);

    if (!result)
    {
        std::wcerr << L"Err: 0x" << std::hex << result.get_result() << std::endl;
        return 1;
    }

    return 0;
}

