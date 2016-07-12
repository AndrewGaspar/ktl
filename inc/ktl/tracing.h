#pragma once

#include "scope.h"
#include <TraceLoggingProvider.h>

#define KtlTraceContext() \
    TraceLoggingWideString((__FUNCTIONW__), "Function"), \
    TraceLoggingWideString((__FILEW__), "File"), \
    TraceLoggingUInt32((__LINE__), "Line")

#define KtlLogEntry(_tlProvider) \
    TraceLoggingWrite( \
        (_tlProvider), \
        "FunctionEntry", \
        TraceLoggingLevel(TRACE_LEVEL_VERBOSE), \
        KtlTraceContext())

#define KtlLogExit(_tlProvider) \
    TraceLoggingWrite( \
        (_tlProvider), \
        "FunctionExit", \
        TraceLoggingLevel(TRACE_LEVEL_VERBOSE), \
        KtlTraceContext())

#define KtlSuccessExit(_tlProvider) \
    KtlLogExit(_tlProvider); \
    return STATUS_SUCCESS

#define KTL_RETURN_IF_NOT_NT_SUCCESS(_tlProvider, _expression) { \
    auto _result = (_expression); \
    if (!NT_SUCCESS(_result)) \
    { \
        TraceLoggingWrite((_tlProvider), "KtlNotNtSuccess", TraceLoggingNTStatus(_result, "Result"), TraceLoggingWideString(L#_expression, "Expression"), KtlTraceContext()); \
        return _result; \
    } \
}