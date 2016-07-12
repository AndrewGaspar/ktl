#pragma once

#include <guiddef.h>
#include <cstdint>

static constexpr PCWSTR KtlTestDeviceIdentifer = L"KTLTEST";
static constexpr PCWSTR KtlTestDevicePath = L"\\\\.\\KTLTEST";
static constexpr PCWSTR KtlTestDeviceName = L"\\Device\\KTLTEST";
static constexpr PCWSTR KtlTestDeviceSymlink = L"\\DosDevices\\KTLTEST";

enum class KtlTestIoctl : std::uint32_t
{
    ListTests = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS),
    ExecTest = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS),
};