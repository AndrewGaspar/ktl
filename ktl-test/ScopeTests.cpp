#include "Precomp.h"

#include "UnitTest.h"

#include <ktl\scope.h>

TEST_FUNCTION(ScopeTestBasic)
{
    auto scopeExited = false;

    {
        auto setOnExit = ktl::make_scope_exit_paged([&]() {
            scopeExited = true;
        });

    }

    if (!scopeExited) return STATUS_ASSERTION_FAILURE;

    return STATUS_SUCCESS;
}

TEST_FUNCTION(ScopeTestMove)
{
    auto scopeExited = false;

    {
        auto movedExit = [&]() {
            return ktl::make_scope_exit_paged([&]() {
                scopeExited = true;
            });
        }();

        if (scopeExited) return STATUS_ASSERTION_FAILURE;
    }

    if (!scopeExited) return STATUS_ASSERTION_FAILURE;

    return STATUS_SUCCESS;
}

TEST_FUNCTION(Fails)
{
    return STATUS_ASSERTION_FAILURE;
}