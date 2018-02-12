#include "test.hpp"

Metadata Test::meta()
{
	return {
		"#000000",
		"#FFFFFF",
		"http://www.edm2.com/images/thumb/1/13/C%2B%2B.png/250px-C%2B%2B.png",
		"C++ test",
		"Hi",
		"pixel",
		"pixel"
	};
}

Direction Test::move(World world)
{
	return Direction::Left;
}