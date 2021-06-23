#include "Random.h"

#include <random>

#include "Platform.h"

Random::Random()
{
	seed = std::rand();
}

Random::Random(uint32_t seed):
	seed(seed)
{
}


