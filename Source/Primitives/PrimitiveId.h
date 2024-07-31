﻿/*********************************************************************
 * \file   PrimitiveId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_ID_H
#define PRIMITIVE_ID_H

#include "Frameworks/Rtti.h"
#include <string>
#include <vector>

namespace Enigma::Primitives
{
    class PrimitiveId
    {
    public:
        PrimitiveId() : m_name(""), m_sequence(0), m_rtti(nullptr) {}
        PrimitiveId(const std::string& name, const Frameworks::Rtti& rtti);
        PrimitiveId(const std::string& name, std::uint64_t sequence, const Frameworks::Rtti& rtti);
        PrimitiveId(const std::vector<std::string>& tokens);
        ~PrimitiveId() = default;
        PrimitiveId(const PrimitiveId& other);
        PrimitiveId& operator=(const PrimitiveId& other);
        PrimitiveId(PrimitiveId&& other) noexcept;
        PrimitiveId& operator=(PrimitiveId&& other) noexcept;

        bool operator==(const PrimitiveId& other) const { return m_name == other.m_name && m_sequence == other.m_sequence && m_rtti == other.m_rtti; }
        bool operator!=(const PrimitiveId& other) const { return m_name != other.m_name || m_sequence != other.m_sequence || m_rtti != other.m_rtti; }

        bool empty() const { return m_rtti == nullptr || m_name.empty(); }
        std::vector<std::string> tokens() const;
        const std::string& name() const { return m_name; }
        const std::uint64_t sequence() const { return m_sequence; }
        const Frameworks::Rtti& rtti() const { return *m_rtti; }
        bool isOrigin() const { return m_sequence == 0; }
        bool isEqual(const PrimitiveId& other) const { return isOrigin() ? operator==(other.origin()) : operator==(other); }

        PrimitiveId origin() const { return PrimitiveId(m_name, 0, *m_rtti); }
        PrimitiveId next() const;

        class hash
        {
        public:
            size_t operator()(const PrimitiveId& id) const
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

#endif // PRIMITIVE_ID_H
