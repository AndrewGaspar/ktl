#pragma once

#include <ntdef.h>

#pragma section("ktltest$a_header")
#pragma section("ktltest$b_test_metadata")
#pragma section("ktltest$c_footer")

using TestFunction = NTSTATUS(*)();

struct TestMetadata
{
    PCWSTR Name;
    TestFunction Test;
};

EXTERN_C __declspec(allocate("ktltest$a_header")) const TestMetadata __declspec(selectany) TestMetadataHeader = {};
EXTERN_C __declspec(allocate("ktltest$c_footer")) const TestMetadata __declspec(selectany) TestMetadataFooter = {};

inline TestMetadata const * TestBegin()
{ 
    return &TestMetadataHeader + 1;
}

inline TestMetadata const * TestEnd()
{
    return &TestMetadataFooter;
}

#define TEST_FUNCTION(FunctionName) \
    static NTSTATUS FunctionName(); \
    EXTERN_C __declspec(allocate("ktltest$b_test_metadata")) const TestMetadata TestMetadata_ ## FunctionName = { L#FunctionName, &FunctionName }; \
    static NTSTATUS FunctionName()