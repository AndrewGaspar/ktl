#pragma once

#include <ntdef.h>
#include <Tracing.h>
#include <wdf.h>
#include <ktl\wdx.h>
#include <memory>
#include <utility>
#include <type_traits>

class TraceLoggingRegistration
{
    std::unique_ptr<std::remove_reference_t<decltype(*std::declval<TraceLoggingHProvider>())>, decltype(&::TraceLoggingUnregister)> m_provider{ nullptr, ::TraceLoggingUnregister }; 

public:

    TLG_STATUS Register(TraceLoggingHProvider provider)
    {
        auto result = TraceLoggingRegister(provider);
        if (NT_SUCCESS(result))
        {
            m_provider.reset(provider);
        }
        return result;
    }

    TraceLoggingRegistration(TraceLoggingHProvider provider)
    {
        // assume success
        (void)Register(provider);
    }
};

struct DriverContext
{
    TraceLoggingRegistration registration;

    DriverContext(TraceLoggingRegistration&& registration) : registration(std::move(registration))
    {

    }

    ~DriverContext()
    {
        TraceLoggingWrite(KtlTestTracing, "DriverUnload", KtlTraceContext());
    }
};

WDX_DECLARE_CONTEXT_TYPE_WITH_NAME(DriverContext, GetDriverContext);

