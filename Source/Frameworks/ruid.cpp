#include "ruid.h"
#include <chrono>
#include <random>

using namespace Enigma::Frameworks;

using hclock = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<std::uint64_t>;

bool s_hasInitialized = false;
static std::random_device s_seed;
static std::default_random_engine s_generator;
static std::uniform_int_distribution<std::uint64_t> s_unit64_rand;

Ruid Ruid::generate()
{
    auto now = hclock::now();
    duration curr = duration(now.time_since_epoch().count());
    std::uint64_t second = curr.count();
    if (!s_hasInitialized)
    {
        s_generator = std::default_random_engine{ s_seed() };
        s_hasInitialized = true;
    }
    std::uint64_t first = s_unit64_rand(s_generator);
    return Ruid(first, second);
}

bool Ruid::operator==(const Ruid& ruid) const
{
    return ((m_first == ruid.m_first) && (m_second == ruid.m_second));
}

bool Ruid::operator!=(const Ruid& ruid) const
{
    return ((m_first != ruid.m_first) || (m_second != ruid.m_second));
}
