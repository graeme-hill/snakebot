#include <string>
#include <unordered_map>
#include "snakelib.hpp"

class Algorithms
{
public:
    Algorithms();
    static Algorithm *get(std::string key);

private:
    static Algorithms _instance;
    std::unordered_map<std::string, std::unique_ptr<Algorithm>> _algorithms;
};
