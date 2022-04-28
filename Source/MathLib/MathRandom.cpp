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

double MathRandom::IntervalRandom(double minA, double maxA)
{
    std::uniform_real_distribution<double> unif_rand(minA, maxA);
    return unif_rand(s_generator);
}

int MathRandom::IntervalRandom(int minA, int maxA)
{
    std::uniform_int_distribution<int> unif_rand(minA, maxA);
    return unif_rand(s_generator);
}
