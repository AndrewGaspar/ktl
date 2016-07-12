#pragma once

#include "utility.h"

#define SCOPE_EXIT_TYPE_NAME ScopeExitPagedImpl
#define SCOPE_EXIT_CODE_SEGMENT "PAGE"

#include "details\scope_impl.h"

#undef SCOPE_EXIT_TYPE_NAME
#undef SCOPE_EXIT_CODE_SEGMENT

#define SCOPE_EXIT_TYPE_NAME ScopeExitNonpagedImpl
#define SCOPE_EXIT_CODE_SEGMENT ".text"

#include "details\scope_impl.h"

#undef SCOPE_EXIT_TYPE_NAME
#undef SCOPE_EXIT_CODE_SEGMENT

namespace ktl
{
    template<typename F>
    auto PAGED make_scope_exit_paged(F&& f)
    {
        return details::ScopeExitPagedImpl<F>(ktl::move(f));
    }

    template<typename F>
    auto NONPAGED make_scope_exit_nonpaged(F&& f)
    {
        return details::ScopeExitNonpagedImpl<F>(ktl::move(f));
    }
}