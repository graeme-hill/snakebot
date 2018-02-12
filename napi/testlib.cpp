#include "testlib.hpp"

napi_value Assert::_assertEqualFn;

void Assert::equal(std::string actual, std::string expected)
{

}

void Assert::equal(uint32_t actual, uint32_t expected)
{

}

void Assert::setCallbacks(napi_value assertEqualFn)
{
	_assertEqualFn = assertEqualFn;
}
