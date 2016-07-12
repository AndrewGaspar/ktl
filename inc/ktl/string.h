#pragma once

#include <ntdef.h>
#include <cstdint>
#include <algorithm>

namespace ktl
{
    constexpr std::size_t cx_wcslen(_In_ PCWSTR str)
    {
        return *str == L'\0' ? 0 : cx_wcslen(str + 1) + 1;
    }

    namespace details
    {
        constexpr UNICODE_STRING make_const_unicode_string(std::uint16_t length, PCWSTR str)
        {
            return{ length * sizeof(wchar_t), (length + 1) * sizeof(wchar_t), const_cast<PWCH>(str) };
        }
    }

    constexpr UNICODE_STRING make_const_unicode_string(PCWSTR str)
    {
        return details::make_const_unicode_string(static_cast<std::uint16_t>(cx_wcslen(str)), str);
    }
}