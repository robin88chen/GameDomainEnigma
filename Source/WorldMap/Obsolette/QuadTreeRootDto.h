/*********************************************************************
 * \file   QuadTreeRootDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef QUAD_TREE_ROOT_DTO_H
#define QUAD_TREE_ROOT_DTO_H

#include "GameEngine/GenericDto.h"
#include "GameEngine/FactoryDesc.h"

namespace Enigma::WorldMap
{
    class QuadTreeRootDto
    {
    public:
        QuadTreeRootDto();

        [[nodiscard]] const std::string& name() const { return m_name; }
        [[nodiscard]] std::string& name() { return m_name; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }
        [[nodiscard]] const Engine::GenericDto& root() const { return m_root; }
        [[nodiscard]] Engine::GenericDto& root() { return m_root; }

        static QuadTreeRootDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factoryDesc;
        Engine::GenericDto m_root;
    };
}

#endif // QUAD_TREE_ROOT_DTO_H
