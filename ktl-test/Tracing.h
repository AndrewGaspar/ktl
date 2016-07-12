#pragma once

#include <evntrace.h>
#include <ktl\tracing.h>

TRACELOGGING_DECLARE_PROVIDER(KtlTestTracing);

#define LogEntry() KtlLogEntry(KtlTestTracing)
#define LogExit() KtlLogExit(KtlTestTracing)
#define SuccessExit() KtlSuccessExit(KtlTestTracing)

#define RETURN_IF_NOT_NT_SUCCESS(_expression) KTL_RETURN_IF_NOT_NT_SUCCESS(KtlTestTracing, _expression)