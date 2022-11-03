#include "MathRandom.h"
#include <random>

using namespace Enigma::MathLib;

static bool s_hasSeeded = false;
static std::random_device s_randomSeed;
static std::default_random_engine s_generator;
static std::uniform_real_distribution<double> s_unit_rand(0.0, 1.0);
static std::uniform_real_distribution<double> s_symmetric_rand(-1.0, 1.0);

bool MathRandom::HasSeeded()
{
    return s_hasSeeded;
}

void MathRandom::RandomSeed()
{
    s_generator = std::default_random_engine{ s_randomSeed() };
    s_hasSeeded = true;
}

double MathRandom::UnitRandom()
{
    return s_unit_rand(s_generator);
}

double MathRandom::SymmetricRandom()
{
    return s_symmetric_rand(s_generator);
}

UniformDoubleDistribution MathRandom::IntervalDistribution(double minA, double maxA)
{
    return UniformDoubleDistribution(minA, maxA, s_generator);
}

UniformFloatDistribution MathRandom::IntervalDistribution(float minA, float maxA)
{
    return UniformFloatDistribution(minA, maxA, s_generator);
}

UniformIntDistribution MathRandom::IntervalDistribution(int minA, int maxA)
{
    return UniformIntDistribution(minA, maxA, s_generator);
}
