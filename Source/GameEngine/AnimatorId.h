/*********************************************************************
 * \file   AnimatorId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_ID_H
#define ANIMATOR_ID_H

#include "Frameworks/Rtti.h"
#include <string>
#include <vector>

namespace Enigma::Engine
{
    class AnimatorId
    {
    public:
        AnimatorId() = default;
        AnimatorId(const std::string& name, const Frameworks::Rtti& rtti);
        AnimatorId(const std::string& name, std::uint64_t sequence, const Frameworks::Rtti& rtti);
        AnimatorId(const std::vector<std::string>& tokens);
        ~AnimatorId() = default;
        AnimatorId(const AnimatorId& other);
        AnimatorId& operator=(const AnimatorId& other);
        AnimatorId(AnimatorId&& other) noexcept;
        AnimatorId& operator=(AnimatorId&& other) noexcept;

        bool operator==(const AnimatorId& other) const { return m_name == other.m_name && m_sequence == other.m_sequence && m_rtti == other.m_rtti; }
        bool operator!=(const AnimatorId& other) const { return m_name != other.m_name || m_sequence != other.m_sequence || m_rtti != other.m_rtti; }

        std::vector<std::string> tokens() const;
        const std::string& name() const { return m_name; }
        const std::uint64_t sequence() const { return m_sequence; }
        const Frameworks::Rtti& rtti() const { return *m_rtti; }

        AnimatorId origin() const { return AnimatorId(m_name, 0, *m_rtti); }
        AnimatorId next() const;

        class hash
        {
        public:
            size_t operator()(const AnimatorId& id) const
            {
                return std::hash<std::string>()(id.m_name) ^ std::hash<std::uint64_t>()(id.m_sequence);
            }
        };

    private:
        std::string m_name;
        std::uint64_t m_sequence; // sequence number, used to distinguish between objects with the same name
        const Frameworks::Rtti* m_rtti;
    };
}

#endif // ANIMATOR_ID_H
