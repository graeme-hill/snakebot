/* This file serves as the entry point to C++ snakebot implementation and it
 * should be the only place that uses N-API. The move() function gets called
 * from JS and it takes care of converting the JS world object into a C++
 * object that the rest of the code can use. All other cpp/hpp files should
 * just use standard C++.
 */

#include <node_api.h>
#include <assert.h>
#include "snakelib.hpp"
#include "algorithms/test.hpp"
#include <memory>

std::unordered_map<std::string, std::unique_ptr<Algorithm>> algorithms;

#define HANDLE_ERROR(msg) if (status != napi_ok) napi_throw_error(env, NULL, msg)

#define DECLARE_NAPI_METHOD(name, func) \
    { name, 0, func, 0, 0, 0, napi_default, 0 }

std::string directionToString(Direction direction)
{
    switch (direction)
    {
        case Direction::Up: return "up";
        case Direction::Down: return "down";
        case Direction::Left: return "left";
        default: return "right";
    }
}

Point makePoint(napi_env env, napi_value jsFood)
{
    napi_value xProp;
    napi_create_string_utf8(env, "x", NAPI_AUTO_LENGTH, &xProp);

    napi_value yProp;
    napi_create_string_utf8(env, "y", NAPI_AUTO_LENGTH, &yProp);

    napi_value jsX;
    napi_get_property(env, jsFood, xProp, &jsX);

    napi_value jsY;
    napi_get_property(env, jsFood, yProp, &jsY);

    uint32_t x;
    napi_get_value_uint32(env, jsX, &x);

    uint32_t y;
    napi_get_value_uint32(env, jsY, &y);

    return { x, y };
}

Snake makeSnake(napi_env env, napi_value jsSnake)
{
    Snake snake {};

    napi_value idProp;
    napi_create_string_utf8(env, "id", NAPI_AUTO_LENGTH, &idProp);

    napi_value healthProp;
    napi_create_string_utf8(env, "health", NAPI_AUTO_LENGTH, &healthProp);

    napi_value partsProp;
    napi_create_string_utf8(env, "body", NAPI_AUTO_LENGTH, &partsProp);

    napi_value dataProp;
    napi_create_string_utf8(env, "data", NAPI_AUTO_LENGTH, &dataProp);

    napi_value jsId;
    napi_get_property(env, jsSnake, idProp, &jsId);

    napi_value jsHealth;
    napi_get_property(env, jsSnake, healthProp, &jsHealth);

    napi_value jsParts;
    napi_get_property(env, jsSnake, partsProp, &jsParts);

    napi_value jsPartsArray;
    napi_get_property(env, jsParts, dataProp, &jsPartsArray);

    char idBuffer[100];
    napi_get_value_string_utf8(env, jsId, idBuffer, 100, NULL);
    snake.id = std::string(idBuffer);

    napi_get_value_uint32(env, jsHealth, &snake.health);

    uint32_t partCount;
    napi_get_array_length(env, jsPartsArray, &partCount);

    for (uint32_t i = 0; i < partCount; i++)
    {
        napi_value jsPoint;
        napi_get_element(env, jsPartsArray, i, &jsPoint);
        Point point = makePoint(env, jsPoint);
        snake.parts.push_back(point);
    }

    return snake;
}

World makeWorld(napi_env env, napi_value jsWorld)
{
    World world {};

    napi_value foodProp;
    napi_create_string_utf8(env, "food", NAPI_AUTO_LENGTH, &foodProp);

    napi_value dataProp;
    napi_create_string_utf8(env, "data", NAPI_AUTO_LENGTH, &dataProp);

    napi_value snakesProp;
    napi_create_string_utf8(env, "snakes", NAPI_AUTO_LENGTH, &snakesProp);

    napi_value youProp;
    napi_create_string_utf8(env, "you", NAPI_AUTO_LENGTH, &youProp);

    napi_value jsFood;
    napi_get_property(env, jsWorld, foodProp, &jsFood);

    napi_value jsfoodArray;
    napi_get_property(env, jsFood, dataProp, &jsfoodArray);

    napi_value jsSnakes;
    napi_get_property(env, jsWorld, snakesProp, &jsSnakes);

    napi_value jsSnakesArray;
    napi_get_property(env, jsSnakes, dataProp, &jsSnakesArray);

    napi_value jsYou;
    napi_get_property(env, jsWorld, youProp, &jsYou);

    uint32_t foodCount;
    napi_get_array_length(env, jsfoodArray, &foodCount);

    uint32_t snakesCount;
    napi_get_array_length(env, jsSnakesArray, &snakesCount);

    for (uint32_t i = 0; i < foodCount; i++)
    {
        napi_value jsFoodObj;
        napi_get_element(env, jsfoodArray, i, &jsFoodObj);

        Point foodPoint = makePoint(env, jsFoodObj);
        world.food.push_back(foodPoint);
    }

    for (uint32_t i = 0; i < snakesCount; i++)
    {
        napi_value jsSnakeObj;
        napi_get_element(env, jsSnakesArray, i, &jsSnakeObj);

        Snake snake = makeSnake(env, jsSnakeObj);
        world.snakes.push_back(snake);
    }

    Snake youSnake = makeSnake(env, jsYou);
    world.you = youSnake.id;

    return world;
}

napi_value metadataToJsValue(napi_env env, Metadata metadata)
{
    napi_value color, secondaryColor, headUrl, name, taunt, headType, tailType;
    napi_create_string_utf8(
        env, metadata.color.c_str(), NAPI_AUTO_LENGTH, &color);
    napi_create_string_utf8(
        env, metadata.secondaryColor.c_str(), NAPI_AUTO_LENGTH, &secondaryColor);
    napi_create_string_utf8(
        env, metadata.headUrl.c_str(), NAPI_AUTO_LENGTH, &headUrl);
    napi_create_string_utf8(
        env, metadata.name.c_str(), NAPI_AUTO_LENGTH, &name);
    napi_create_string_utf8(
        env, metadata.taunt.c_str(), NAPI_AUTO_LENGTH, &taunt);
    napi_create_string_utf8(
        env, metadata.headType.c_str(), NAPI_AUTO_LENGTH, &headType);
    napi_create_string_utf8(
        env, metadata.tailType.c_str(), NAPI_AUTO_LENGTH, &tailType);

    napi_value obj;
    napi_create_object(env, &obj);

    napi_set_named_property(env, obj, "color", color);
    napi_set_named_property(env, obj, "secondary_color", secondaryColor);
    napi_set_named_property(env, obj, "head_url", headUrl);
    napi_set_named_property(env, obj, "name", name);
    napi_set_named_property(env, obj, "taunt", taunt);
    napi_set_named_property(env, obj, "head_type", headType);
    napi_set_named_property(env, obj, "tail_type", tailType);

    return obj;
}

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
    std::array<napi_property_descriptor, 2> descriptors;
    descriptors[0] = DECLARE_NAPI_METHOD("move", move);
    descriptors[1] = DECLARE_NAPI_METHOD("meta", meta);
    status = napi_define_properties(
        env, exports, descriptors.size(), descriptors.data());
    HANDLE_ERROR("Failed to export move and meta functions from C++ to JS");
    return exports;
}

NAPI_MODULE(snaklebot_native, init)