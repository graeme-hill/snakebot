#include <iostream>
#include <string>
#include <sstream>
#include "timing.hpp"
#include "snakelib.hpp"
#include "dispatcher.hpp"
#include "json.hpp"

Algorithm *Dispatcher::algorithm = nullptr;

std::string getGameId(std::string json)
{
    auto j = nlohmann::json::parse(json);
    auto id = j["game_id"];
    std::stringstream ss;
    ss << id;
    return ss.str();
}

Snake getSnake(nlohmann::json jSnake)
{
    Snake s;

    s.id = jSnake["id"];
    s.health = jSnake["health"];
    s.dead = s.health == 0 ? true : false;

    for (auto &jPart : jSnake["body"]["data"])
    {
        s.parts.push_back({ jPart["x"], jPart["y"] });
    }

    return s;
}

World getWorld(std::string json)
{
    World w;
    auto j = nlohmann::json::parse(json);

    w.width = j["width"];
    w.height = j["height"];

    // coerce id to string
    std::stringstream idss;
    idss << j["id"];
    w.id = idss.str();

    for (auto &jFood : j["food"]["data"])
    {
        w.food.push_back({ jFood["x"], jFood["y"] });
    }

    w.snakes.push_back(getSnake(j["you"]));
    w.you = w.snakes[0].id;

    for (auto &jSnake : j["snakes"]["data"])
    {
        // Make sure my snake (you) doesn't get added twice.
        std::string thisId = jSnake["id"];
        if (thisId == w.you)
            continue;
        
        //don't bother with dead snakes
        if(jSnake["health"] > 0) {
            w.snakes.push_back(getSnake(jSnake));
        }
        
    }

    return w;
}

std::string Dispatcher::move(std::string json)
{
    auto start = Clock::now();

    World world = getWorld(json);
    GameState state(world);
    Direction direction = Dispatcher::algorithm->move(state);

    nlohmann::json jsonResult = {
        { "move", directionToString(direction) }
    };

    auto end = Clock::now();
    Seconds diff = end - start;
    auto millis = diff.count() * 1000.0;

    std::cout << millis << " millis" << std::endl;

    return jsonResult.dump();
}

std::string Dispatcher::start(std::string json)
{
    std::string id = getGameId(json);
    Metadata meta = Dispatcher::algorithm->meta();
    Dispatcher::algorithm->start(id);

    nlohmann::json jsonResult = {
        { "color", meta.color },
        { "secondary_color", meta.secondaryColor },
        { "head_url", meta.headUrl },
        { "name", meta.name },
        { "taunt", meta.taunt },
        { "head_type", meta.headType },
        { "tail_type", meta.tailType }
    };

    return jsonResult.dump();
}

//curl -d '{"game_id": 1234}' -H "Content-Type: application/json" -X POST http://localhost:5000/start
