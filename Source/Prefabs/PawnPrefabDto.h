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

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        [[nodiscard]] bool isTopLevel() const { return m_isTopLevel; }
        bool& isTopLevel() { return m_isTopLevel; }
        const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] MathLib::Matrix4 localTransform() const { return m_localTransform; }
        MathLib::Matrix4& localTransform() { return m_localTransform; }
        [[nodiscard]] MathLib::Matrix4 worldTransform() const { return m_worldTransform; }
        MathLib::Matrix4& worldTransform() { return m_worldTransform; }
        [[nodiscard]] unsigned int graphDepth() const { return m_graphDepth; }
        unsigned int& graphDepth() { return m_graphDepth; }
        [[nodiscard]] Engine::GenericDto worldBound() const { return m_worldBound; }
        Engine::GenericDto& worldBound() { return m_worldBound; }
        [[nodiscard]] unsigned int cullingMode() const { return m_cullingMode; }
        unsigned int& cullingMode() { return m_cullingMode; }
        [[nodiscard]] unsigned int spatialFlag() const { return m_spatialFlag; }
        unsigned int& spatialFlag() { return m_spatialFlag; }
        [[nodiscard]] unsigned int notifyFlag() const { return m_notifyFlag; }
        unsigned int& notifyFlag() { return m_notifyFlag; }

        static PawnPrefabDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

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
    };
}

#endif // PAWN_PREFAB_DTO_H
