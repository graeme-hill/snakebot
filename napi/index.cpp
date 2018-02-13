/* This file serves as the entry point to C++ snakebot implementation and it
 * should be the only place that uses N-API. The move() function gets called
 * from JS and it takes care of converting the JS world object into a C++
 * object that the rest of the code can use. All other cpp/hpp files should
 * just use standard C++.
 */

#include "interop.hpp"
#include "snakelib.hpp"
#include "algorithms/test.hpp"
#include "test/testsuite.hpp"
#include <memory>

std::unordered_map<std::string, std::unique_ptr<Algorithm>> algorithms;

napi_value meta(napi_env env, napi_callback_info info)
{
    napi_status status;

    // Buffer to store arguments passed from JS land.
    std::array<napi_value, 1> args;
    size_t argCount = args.size();

    // Fetch the arguments into args. Both reads AND writes argCount.
    status = napi_get_cb_info(env, info, &argCount, args.data(), NULL, NULL);
    HANDLE_ERROR("Cannot read args passed to move()");

    napi_value jsAlgorithm = args[0];

    char algorithmBuffer[200];
    napi_get_value_string_utf8(env, jsAlgorithm, algorithmBuffer, 100, NULL);
    std::string algorithmName(algorithmBuffer);

    auto algoIter = algorithms.find(algorithmName);
    if (algoIter == algorithms.end())
    {
        napi_throw_error(env, NULL, "C++ algorithm not found");
    }
    Metadata metadata = (*algoIter).second->meta();
    return metadataToJsValue(env, metadata);
}

napi_value move(napi_env env, napi_callback_info info)
{
    napi_status status;

    // Buffer to store arguments passed from JS land.
    std::array<napi_value, 2> args;
    size_t argCount = args.size();

    // Fetch the arguments into args. Both reads AND writes argCount.
    status = napi_get_cb_info(env, info, &argCount, args.data(), NULL, NULL);
    HANDLE_ERROR("Cannot read args passed to move()");

    napi_value jsWorld = args[0];
    napi_value jsAlgorithm = args[1];

    char algorithmBuffer[200];
    napi_get_value_string_utf8(env, jsAlgorithm, algorithmBuffer, 100, NULL);
    std::string algorithmName(algorithmBuffer);

    auto algoIter = algorithms.find(algorithmName);
    if (algoIter == algorithms.end())
    {
        napi_throw_error(env, NULL, "C++ algorithm not found");
    }

    World world = makeWorld(env, jsWorld);
    Direction direction = (*algoIter).second->move(world);
    std::string directionStr = directionToString(direction);

    napi_value jsDirection;
    napi_create_string_utf8(env, directionStr.c_str(), directionStr.size(), &jsDirection);

    return jsDirection;
}

napi_value test(napi_env env, napi_callback_info info)
{
    napi_status status;

    // Buffer to store arguments passed from JS land.
    std::array<napi_value, 1> args;
    size_t argCount = args.size();

    // Fetch the arguments into args. Both reads AND writes argCount.
    status = napi_get_cb_info(env, info, &argCount, args.data(), NULL, NULL);
    HANDLE_ERROR("Cannot read args passed to move()");

    napi_value jsCallbacks = args[0];

    // Initialize Assert class
    napi_value assertEqualFn, parseWorldFn;
    napi_get_named_property(env, jsCallbacks, "assertEqual", &assertEqualFn);
    napi_get_named_property(env, jsCallbacks, "parseWorld", &parseWorldFn);
    Interop::setCallbacks(env, assertEqualFn, parseWorldFn);

    // Start tests
    TestSuite::run();

    // Return undefined (ie: no return value)
    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

napi_value init(napi_env env, napi_value exports)
{
    // INITIALIZE AVAILABLE ALGORITHMS HERE!
    algorithms["test"] = std::unique_ptr<Algorithm>(new Test());

    // Make the move() function above available to be called by JS code.
    // Instead of exporting every algorithm's move function, just export this
    // one function and let it act as a dispatcher using second parameter as
    // the algorithm name (which needs to match the name used as the key in
    // algorithms unordered map.
    napi_status status;
    std::array<napi_property_descriptor, 3> descriptors;
    descriptors[0] = DECLARE_NAPI_METHOD("move", move);
    descriptors[1] = DECLARE_NAPI_METHOD("meta", meta);
    descriptors[2] = DECLARE_NAPI_METHOD("test", test);
    status = napi_define_properties(
        env, exports, descriptors.size(), descriptors.data());
    HANDLE_ERROR("Failed to export move and meta functions from C++ to JS");
    return exports;
}

NAPI_MODULE(snaklebot_native, init)