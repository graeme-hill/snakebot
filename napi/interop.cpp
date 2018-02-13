#include "interop.hpp"
#include <array>
#include <iostream>

napi_value Interop::_assertEqualFn;
napi_value Interop::_parseWorldFn;
napi_env Interop::_env;

void Interop::setCallbacks(napi_env env, napi_value assertEqualFn, napi_value parseWorldFn)
{
	_env = env;
	_assertEqualFn = assertEqualFn;
	_parseWorldFn = parseWorldFn;
}

void assertEqual(napi_value jsActual, napi_value jsExpected, std::string message)
{
	napi_env env = Interop::env();

	napi_value jsMessage, jsThis;
	napi_create_string_utf8(env, message.c_str(), NAPI_AUTO_LENGTH, &jsMessage);
	napi_get_null(env, &jsThis);

	std::array<napi_value, 3> args;
	args[0] = jsActual;
	args[1] = jsExpected;
	args[2] = jsMessage;

	napi_call_function(env, jsThis, Interop::assertEqualFn(), args.size(), args.data(), NULL);
}

void assertEqual(std::string actual, std::string expected, std::string message)
{
	napi_env env = Interop::env();
	napi_value jsActual, jsExpected;
	napi_create_string_utf8(env, actual.c_str(), NAPI_AUTO_LENGTH, &jsActual);
	napi_create_string_utf8(env, expected.c_str(), NAPI_AUTO_LENGTH, &jsExpected);
	assertEqual(jsActual, jsExpected, message);	
}

void assertEqual(uint32_t actual, uint32_t expected, std::string message)
{
	napi_env env = Interop::env();
	napi_value jsActual, jsExpected;
	napi_create_uint32(env, actual, &jsActual);
	napi_create_uint32(env, expected, &jsExpected);
	assertEqual(jsActual, jsExpected, message);	
}

void assertEqual(Direction actual, Direction expected, std::string message)
{
    std::string actualStr = directionToString(actual);
    std::string expectedStr = directionToString(expected);
    assertEqual(actualStr, expectedStr, message);
}

void assertTrue(bool actual, std::string message)
{
	napi_env env = Interop::env();
	napi_value jsActual, jsExpected;
	napi_get_boolean(env, actual, &jsActual);
	napi_get_boolean(env, true, &jsExpected);
	assertEqual(jsActual, jsExpected, message);	
}

World parseWorld(std::vector<std::string> rows)
{
	napi_status status;
	napi_env env = Interop::env();

	napi_value jsRowsArray, jsThis;
	status = napi_create_array(env, &jsRowsArray);
	HANDLE_ERROR("cannot create jsRowsArray");
	status = napi_get_null(env, &jsThis);
	HANDLE_ERROR("cannot get null");

	for (size_t i = 0; i < rows.size(); i++)
	{
		napi_value jsRowStr;
		status = napi_create_string_utf8(env, rows[i].c_str(), NAPI_AUTO_LENGTH, &jsRowStr);
		HANDLE_ERROR("cannot create jsRowStr");
		status = napi_set_element(env, jsRowsArray, i, jsRowStr);
		HANDLE_ERROR("cannot add jsRowStr to jsRowsArray");
	}

	std::array<napi_value, 1> args;
	args[0] = jsRowsArray;

	napi_value jsWorld;
	status = napi_call_function(env, jsThis, Interop::parseWorldFn(), args.size(), args.data(), &jsWorld);
	HANDLE_ERROR("cannot call parseWorldFn");
	
	World world = makeWorld(env, jsWorld);
	return world;
}

Point makePoint(napi_env env, napi_value jsFood)
{
    napi_value jsX;
    napi_get_named_property(env, jsFood, "x", &jsX);

    napi_value jsY;
    napi_get_named_property(env, jsFood, "y", &jsY);

    uint32_t x;
    napi_get_value_uint32(env, jsX, &x);

    uint32_t y;
    napi_get_value_uint32(env, jsY, &y);

    return { x, y };
}

Snake makeSnake(napi_env env, napi_value jsSnake)
{
    Snake snake {};

    napi_value jsId;
    napi_get_named_property(env, jsSnake, "id", &jsId);

    napi_value jsHealth;
    napi_get_named_property(env, jsSnake, "health", &jsHealth);

    napi_value jsParts;
    napi_get_named_property(env, jsSnake, "body", &jsParts);

    napi_value jsPartsArray;
    napi_get_named_property(env, jsParts, "data", &jsPartsArray);

    char idBuffer[100];
    size_t idBufferSize;
    napi_get_value_string_utf8(env, jsId, idBuffer, 100, &idBufferSize);
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

    napi_value jsFood, jsfoodArray, jsSnakes, jsSnakesArray, jsYou, jsWidth, jsHeight;
    napi_get_named_property(env, jsWorld, "food", &jsFood);
    napi_get_named_property(env, jsFood, "data", &jsfoodArray);
    napi_get_named_property(env, jsWorld, "snakes", &jsSnakes);
    napi_get_named_property(env, jsSnakes, "data", &jsSnakesArray);
    napi_get_named_property(env, jsWorld, "you", &jsYou);
    napi_get_named_property(env, jsWorld, "width", &jsWidth);
    napi_get_named_property(env, jsWorld, "height", &jsHeight);

    napi_get_value_uint32(env, jsWidth, &world.width);
    napi_get_value_uint32(env, jsHeight, &world.height);

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
    napi_value color, secondaryColor, headUrl, name, taunt, headType, tailType, obj;
    napi_create_string_utf8(env, metadata.color.c_str(), NAPI_AUTO_LENGTH, &color);
    napi_create_string_utf8(env, metadata.secondaryColor.c_str(), NAPI_AUTO_LENGTH, &secondaryColor);
    napi_create_string_utf8(env, metadata.headUrl.c_str(), NAPI_AUTO_LENGTH, &headUrl);
    napi_create_string_utf8(env, metadata.name.c_str(), NAPI_AUTO_LENGTH, &name);
    napi_create_string_utf8(env, metadata.taunt.c_str(), NAPI_AUTO_LENGTH, &taunt);
    napi_create_string_utf8(env, metadata.headType.c_str(), NAPI_AUTO_LENGTH, &headType);
    napi_create_string_utf8(env, metadata.tailType.c_str(), NAPI_AUTO_LENGTH, &tailType);

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