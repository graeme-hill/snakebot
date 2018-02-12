#pragma once

#include <string>
#include <cstdint>
#include <node_api.h>

class Assert
{
public:
    static void equal(std::string actual, std::string expected);
    static void equal(uint32_t actual, uint32_t expected);
    static void setCallbacks(napi_value assertEqualFn);

private:
    static napi_value _assertEqualFn;
};