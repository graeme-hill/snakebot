#pragma once

#include <string>
#include <sstream>
#include <cstdint>
#include <node_api.h>
#include <vector>
#include "snakelib.hpp"
#include "simulator.hpp"

#define HANDLE_ERROR(msg) if (status != napi_ok) napi_throw_error(env, NULL, msg)

#define DECLARE_NAPI_METHOD(name, func) \
    { name, 0, func, 0, 0, 0, napi_default, 0 }

class Interop
{
public:
    static void setCallbacks(napi_env env, napi_value assertEqualFn, napi_value parseWorldFn);
    static napi_value assertEqualFn() { return _assertEqualFn; }
    static napi_value parseWorldFn() { return _parseWorldFn; }
    static napi_env env() { return _env; }

private:
    static napi_value _assertEqualFn;
    static napi_value _parseWorldFn;
    static napi_env _env;
};

void assertEqual(std::string actual, std::string expected, std::string message);

void assertEqual(uint32_t actual, uint32_t expected, std::string message);

void assertEqual(Direction actual, Direction expected, std::string message);

void assertEqual(TerminationReason actual, TerminationReason expected, std::string message);

void assertEqual(World &w1, World &w2, std::string messagePart);

void assertEqual(Snake &s1, Snake &s2, std::string messagePart);

void assertEqual(Point p1, Point p2, std::string message);

void assertEqual(Future &f1, Future &f2, std::string msgPart);

void assertEqual(std::vector<Future> &f1, std::vector<Future> &f2, std::string m);

void assertTrue(bool actual, std::string message);

Point makePoint(napi_env env, napi_value jsFood);

Snake makeSnake(napi_env env, napi_value jsSnake);

World makeWorld(napi_env env, napi_value jsWorld);

napi_value metadataToJsValue(napi_env env, Metadata metadata);

World parseWorld(std::vector<std::string> rows);
