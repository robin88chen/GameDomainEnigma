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
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        [[nodiscard]] bool isTopLevel() const { return m_isTopLevel; }
        bool& isTopLevel() { return m_isTopLevel; }
        SpatialId& id() { return m_id; }
        [[nodiscard]] const SpatialId& id() const { return m_id; }
        const std::string& parentName() const { return m_parentName; }
        std::string& parentName() { return m_parentName; }
        [[nodiscard]] MathLib::Matrix4 localTransform() const { return m_localTransform; }
        MathLib::Matrix4& localTransform() { return m_localTransform; }
        [[nodiscard]] MathLib::Matrix4 worldTransform() const { return m_worldTransform; }
        MathLib::Matrix4& worldTransform() { return m_worldTransform; }
        [[nodiscard]] unsigned int graphDepth() const { return m_graphDepth; }
        unsigned int& graphDepth() { return m_graphDepth; }
        [[nodiscard]] Engine::GenericDto modelBound() const { return m_modelBound; }
        Engine::GenericDto& modelBound() { return m_modelBound; }
        [[nodiscard]] Engine::GenericDto worldBound() const { return m_worldBound; }
        Engine::GenericDto& worldBound() { return m_worldBound; }
        [[nodiscard]] unsigned int cullingMode() const { return m_cullingMode; }
        unsigned int& cullingMode() { return m_cullingMode; }
        [[nodiscard]] unsigned int spatialFlag() const { return m_spatialFlag; }
        unsigned int& spatialFlag() { return m_spatialFlag; }
        [[nodiscard]] unsigned int notifyFlag() const { return m_notifyFlag; }
        unsigned int& notifyFlag() { return m_notifyFlag; }
        [[nodiscard]] const std::optional<SpatialId>& parentId() const { return m_parentId; }
        std::optional<SpatialId>& parentId() { return m_parentId; }

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
