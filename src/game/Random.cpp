#include "Random.h"

#include <random>


Random::Random()
{
	seed = std::rand();
}

Random::Random(uint32_t seed):
	seed(seed)
{
}


