/*********************************************************************
 * \file   PawnPrefabDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_PREFAB_DTO_H
#define PAWN_PREFAB_DTO_H

#include "GameEngine/GenericDto.h"
#include "MathLib/Matrix4.h"

namespace Enigma::Prefabs
{
    class PawnPrefabDto
    {
    public:
        PawnPrefabDto();

        [[nodiscard]] const Engine::FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }
        [[nodiscard]] bool IsTopLevel() const { return m_isTopLevel; }
        bool& IsTopLevel() { return m_isTopLevel; }
        const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] MathLib::Matrix4 LocalTransform() const { return m_localTransform; }
        MathLib::Matrix4& LocalTransform() { return m_localTransform; }
        [[nodiscard]] MathLib::Matrix4 WorldTransform() const { return m_worldTransform; }
        MathLib::Matrix4& WorldTransform() { return m_worldTransform; }
        [[nodiscard]] unsigned int GraphDepth() const { return m_graphDepth; }
        unsigned int& GraphDepth() { return m_graphDepth; }
        [[nodiscard]] Engine::GenericDto WorldBound() const { return m_worldBound; }
        Engine::GenericDto& WorldBound() { return m_worldBound; }
        [[nodiscard]] unsigned int CullingMode() const { return m_cullingMode; }
        unsigned int& CullingMode() { return m_cullingMode; }
        [[nodiscard]] unsigned int SpatialFlag() const { return m_spatialFlag; }
        unsigned int& SpatialFlag() { return m_spatialFlag; }
        [[nodiscard]] unsigned int NotifyFlag() const { return m_notifyFlag; }
        unsigned int& NotifyFlag() { return m_notifyFlag; }
        [[nodiscard]] const std::string& ParentName() const { return m_parentName; }
        std::string& ParentName() { return m_parentName; }

        static PawnPrefabDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        bool m_isTopLevel;
        std::string m_name;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_worldTransform;
        unsigned int m_graphDepth;
        Engine::GenericDto m_worldBound;
        unsigned int m_cullingMode;
        unsigned int m_spatialFlag;
        unsigned int m_notifyFlag;
        std::string m_parentName;
    };
}

#endif // PAWN_PREFAB_DTO_H
