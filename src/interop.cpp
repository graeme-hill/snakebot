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

void assertEqual(TerminationReason actual, TerminationReason expected, std::string message)
{
    std::string actualStr = terminationReasonToString(actual);
    std::string expectedStr = terminationReasonToString(expected);
    assertEqual(actualStr, expectedStr, message);
}

void assertEqual(Point p1, Point p2, std::string message)
{
    std::stringstream p1ss;
    std::stringstream p2ss;
    p1ss << "(" << p1.x << "," << p1.y << ")";
    p2ss << "(" << p2.x << "," << p2.y << ")";
    assertEqual(p1ss.str(), p2ss.str(), message);
}

void assertEqual(World &w1, World &w2, std::string messagePart)
{
    assertEqual(w2.width, w1.width, messagePart + " - width");
    assertEqual(w2.height, w1.height, messagePart + " - height");
    assertEqual(w2.food.size(), w1.food.size(), messagePart + " - food count");

    if (w2.food.size() == w1.food.size())
    {
        for (size_t i = 0; i < w2.food.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - food " << i;
            assertEqual(w2.food[i], w1.food[i], msg.str());
        }
    }

    assertEqual(w2.snakes.size(), w1.snakes.size(), messagePart + " - snake count");

    if (w2.snakes.size() == w1.snakes.size())
    {
        for (size_t i = 0; i < w2.snakes.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - snake " << i;
            assertEqual(w2.snakes[i], w1.snakes[i], msg.str());
        }

        if (w2.snakes.size() > 0)
        {
            assertEqual(w2.you, w1.you, messagePart + " - you");
        }
    }
}

void assertEqual(Snake &s1, Snake &s2, std::string messagePart)
{
    assertEqual(s2.id, s1.id, messagePart + " - id");
    assertEqual(s2.length(), s1.length(), messagePart + " - length");

    if (s2.length() == s1.length())
    {
        for (size_t i = 0; i < s2.parts.size(); i++)
        {
            std::stringstream msg;
            msg << messagePart << " - part " << i;
            assertEqual(s2.parts[i], s1.parts[i], msg.str());
        }
    }
}

void assertEqual(Future &f1, Future &f2, std::string msgPart)
{
    assertEqual(f2.obituaries.size(), f1.obituaries.size(), msgPart + " - obit size");

    if (f2.obituaries.size() == f1.obituaries.size())
    {
        for (auto pair : f2.obituaries)
        {
            auto it = f1.obituaries.find(pair.first);
            assertTrue(it != f2.obituaries.end(), msgPart + " - key '" + pair.first + "' is present");
            if (it != f2.obituaries.end())
            {
                assertEqual(pair.second, it->second, msgPart + " - same values");
            }
        }
    }

    assertEqual(f2.foodsEaten.size(), f1.foodsEaten.size(), msgPart + " - foods eaten size");

    if (f2.foodsEaten.size() == f1.foodsEaten.size())
    {
        for (auto pair : f2.foodsEaten)
        {
            auto it = f1.foodsEaten.find(pair.first);
            assertTrue(it != f2.foodsEaten.end(), msgPart + " - key '" + pair.first + "' is present");
            if (it != f2.foodsEaten.end())
            {
                std::vector<uint32_t> turns1 = it->second;
                std::vector<uint32_t> turns2 = pair.second;
                assertEqual(turns2.size(), turns1.size(), msgPart + " - same food count");
                if (turns2.size() == turns1.size())
                {
                    for (size_t i = 0; i < turns2.size(); i++)
                    {
                        std::stringstream ss;
                        assertEqual(turns2[i], turns1[i], msgPart + " - same turn");
                    }
                }
            }
        }
    }

    assertEqual(f2.source.pair.myAlgorithm->meta().name, f1.source.pair.myAlgorithm->meta().name, msgPart + " - algorithm");
    assertTrue(f2.terminationReason == f1.terminationReason, msgPart + " - termination reason");
    assertEqual(f2.turns, f1.turns, msgPart + " - turns");
    assertEqual(f2.move, f1.move, msgPart + " - move");
}

void assertEqual(std::vector<Future> &f1, std::vector<Future> &f2, std::string m)
{
    assertEqual(f2.size(), f1.size(), m + " - future count");

    if (f2.size() == f1.size())
    {
        for (size_t i = 0; i < f2.size(); i++)
        {
            std::stringstream msg;
            msg << m << " - future " << i;
            assertEqual(f2[i], f1[i], msg.str());
        }
    }
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
    snake.dead = false;

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

    napi_value jsFood, jsfoodArray, jsSnakes, jsSnakesArray, jsYou, jsWidth, jsHeight, jsId;
    napi_get_named_property(env, jsWorld, "food", &jsFood);
    napi_get_named_property(env, jsFood, "data", &jsfoodArray);
    napi_get_named_property(env, jsWorld, "snakes", &jsSnakes);
    napi_get_named_property(env, jsSnakes, "data", &jsSnakesArray);
    napi_get_named_property(env, jsWorld, "you", &jsYou);
    napi_get_named_property(env, jsWorld, "width", &jsWidth);
    napi_get_named_property(env, jsWorld, "height", &jsHeight);
	napi_get_named_property(env, jsWorld, "id", &jsId);

    napi_get_value_uint32(env, jsWidth, &world.width);
    napi_get_value_uint32(env, jsHeight, &world.height);

	char idBuffer[100];
    napi_get_value_string_utf8(env, jsId, idBuffer, 100, NULL);
    world.id = std::string(idBuffer);

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

    napi_value jsYouIsTruthy;
    bool youIsTruthy;
    napi_coerce_to_bool(env, jsYou, &jsYouIsTruthy);
    napi_get_value_bool(env, jsYouIsTruthy, &youIsTruthy);
    if (youIsTruthy)
    {
        Snake youSnake = makeSnake(env, jsYou);
        world.you = youSnake.id;
    }
    else
    {
        world.you = "";
    }

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
