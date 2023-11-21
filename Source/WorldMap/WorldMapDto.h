/*********************************************************************
 * \file   WorldMapDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_DTO_H
#define WORLD_MAP_DTO_H
#include "SceneGraph/PortalDtos.h"

namespace Enigma::WorldMap
{
    class WorldMapDto
    {
    public:
        WorldMapDto();

        [[nodiscard]] const std::string& name() const { return m_name; }
        [[nodiscard]] std::string& name() { return m_name; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factory_desc; }
        [[nodiscard]] Engine::FactoryDesc& factoryDesc() { return m_factory_desc; }
        [[nodiscard]] const Engine::GenericDtoCollection& quadTreeRoots() const { return m_quadTreeRoots; }
        [[nodiscard]] Engine::GenericDtoCollection& quadTreeRoots() { return m_quadTreeRoots; }
        [[nodiscard]] const Engine::GenericDto& portalRoot() const { return m_portalRoot; }
        [[nodiscard]] Engine::GenericDto& portalRoot() { return m_portalRoot; }

        static WorldMapDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factory_desc;
        Engine::GenericDtoCollection m_quadTreeRoots;
        Engine::GenericDto m_portalRoot;
    };
}

#endif // WORLD_MAP_DTO_H
