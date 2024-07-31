﻿/*********************************************************************
 * \file   SpatialId.h
 * \brief  scene graph spatial id, used to identify a spatial object
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SPATIAL_ID_H
#define SPATIAL_ID_H

#include "Frameworks/Rtti.h"
#include <string>
#include <vector>

namespace Enigma::SceneGraph
{
    class SpatialId
    {
    public:
        SpatialId() : m_name(""), m_rtti(nullptr) {}
        SpatialId(const std::string& name, const Frameworks::Rtti& rtti);
        SpatialId(const std::vector<std::string>& tokens);
        ~SpatialId();
        SpatialId(const SpatialId& other);
        SpatialId& operator=(const SpatialId& other);
        SpatialId(SpatialId&& other) noexcept;
        SpatialId& operator=(SpatialId&& other) noexcept;

        bool operator==(const SpatialId& other) const { return m_name == other.m_name && m_rtti == other.m_rtti; }
        bool operator!=(const SpatialId& other) const { return m_name != other.m_name || m_rtti != other.m_rtti; }

        bool empty() const { return m_rtti == nullptr; }
        std::vector<std::string> tokens() const;
        const std::string& name() const { return m_name; }
        const Frameworks::Rtti& rtti() const { return *m_rtti; }

        class hash
        {
        public:
            size_t operator()(const SpatialId& id) const
            {
                return std::hash<std::string>()(id.m_name) ^ std::hash<std::uint64_t>()(reinterpret_cast<std::uint64_t>(id.m_rtti));
            }
        };

    private:
        std::string m_name;
        const Frameworks::Rtti* m_rtti;
    };
}

#endif // SPATIAL_ID_H
