/*********************************************************************
 * \file   ruid.h
 * \brief  run-time unique id, 128 bits id, uniform random uint64
 * plus uint64 generate time (hi resolution), 理論上 run-time 是 unique
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RUID_H
#define RUID_H

#include <cstdint>

namespace Enigma::Frameworks
{
    class Ruid
    {
    public:
        Ruid() = default;
        Ruid(std::uint64_t first, std::uint64_t second) : m_first(first), m_second(second) {}
        Ruid(const Ruid&) = default;
        Ruid(Ruid&&) = default;

        Ruid& operator=(const Ruid&) = default;
        Ruid& operator=(Ruid&&) = default;

        static Ruid Generate();

        bool operator==(const Ruid& ruid) const;
        bool operator!=(const Ruid& ruid) const;

    private:
        std::uint64_t m_first;
        std::uint64_t m_second;
    };
}

#endif // RUID_H
