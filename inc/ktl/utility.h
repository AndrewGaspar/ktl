#pragma once

#include "type_traits.h"
#include "code_annotation.h"

namespace ktl
{
    template<typename T>
    constexpr NONPAGED ktl::remove_reference_t<T>&& move(T&& value)
    {
        return static_cast<ktl::remove_reference_t<T>&&>(value);
    }
}