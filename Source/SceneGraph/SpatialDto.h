/*****************************************************************
 * \file   SpatialDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef SPATIAL_DTO_H
#define SPATIAL_DTO_H

#include "MathLib/Matrix4.h"
#include "GameEngine/GenericDto.h"
#include "SpatialId.h"
#include <optional>

namespace Enigma::SceneGraph
{
    class SpatialDto
    {
    public:
        SpatialDto();
        SpatialDto(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& fd) { m_factoryDesc = fd; }
        [[nodiscard]] bool isTopLevel() const { return m_isTopLevel; }
        void isTopLevel(bool is_top) { m_isTopLevel = is_top; }
        void id(const SpatialId& id) { m_id = id; }
        [[nodiscard]] const SpatialId& id() const { return m_id; }
        const std::string& parentName() const { return m_parentName; }
        void parentName(const std::string& name) { m_parentName = name; }
        [[nodiscard]] const MathLib::Matrix4& localTransform() const { return m_localTransform; }
        void localTransform(const MathLib::Matrix4& mx) { m_localTransform = mx; }
        [[nodiscard]] const MathLib::Matrix4& worldTransform() const { return m_worldTransform; }
        void worldTransform(const MathLib::Matrix4& mx) { m_worldTransform = mx; }
        [[nodiscard]] unsigned int graphDepth() const { return m_graphDepth; }
        void graphDepth(unsigned d) { m_graphDepth = d; }
        [[nodiscard]] const Engine::GenericDto& modelBound() const { return m_modelBound; }
        void modelBound(const Engine::GenericDto& b) { m_modelBound = b; }
        [[nodiscard]] const Engine::GenericDto& worldBound() const { return m_worldBound; }
        void worldBound(const Engine::GenericDto& b) { m_worldBound = b; }
        [[nodiscard]] unsigned int cullingMode() const { return m_cullingMode; }
        void cullingMode(unsigned m) { m_cullingMode = m; }
        [[nodiscard]] unsigned int spatialFlag() const { return m_spatialFlag; }
        void spatialFlag(unsigned s) { m_spatialFlag = s; }
        [[nodiscard]] unsigned int notifyFlag() const { return m_notifyFlag; }
        void notifyFlag(unsigned n) { m_notifyFlag = n; }
        [[nodiscard]] const std::optional<SpatialId>& parentId() const { return m_parentId; }
        void parentId(const SpatialId& id) { m_parentId = id; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        bool m_isTopLevel;
        SpatialId m_id;
        std::string m_parentName;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_worldTransform;
        unsigned int m_graphDepth;
        Engine::GenericDto m_modelBound;
        Engine::GenericDto m_worldBound;
        unsigned int m_cullingMode;
        unsigned int m_spatialFlag;
        unsigned int m_notifyFlag;
        std::optional<SpatialId> m_parentId;
    };
}

#endif // SPATIAL_DTO_H
