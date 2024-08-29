/*********************************************************************
 * \file   SkinOperatorAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef SKIN_OPERATOR_ASSEMBLER_H
#define SKIN_OPERATOR_ASSEMBLER_H

#include "GameEngine/GenericDto.h"
#include "MathLib/Matrix4.h"
#include "Primitives/PrimitiveId.h"

namespace Enigma::Renderables
{
    class SkinOperatorAssembler
    {
    public:
        SkinOperatorAssembler();

        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }
        void operatedSkin(const Primitives::PrimitiveId& id);
        void skinNodeName(const std::string& name);
        void addBone(const std::string& name, const MathLib::Matrix4& t_pos_offset);
        void addBone(const std::string& name);
        void bones(const std::vector<std::string>& names, const std::vector<MathLib::Matrix4>& t_pos_offsets);
        void bones(const std::vector<std::string>& names);

        Engine::GenericDto assemble() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_skinMeshId;
        std::optional<std::string> m_skinMeshNodeName;
        std::vector<std::string> m_boneNodeNames;
        std::optional<std::vector<MathLib::Matrix4>> m_t_posOffsets;
    };

    class SkinOperatorDisassembler
    {
    public:
        SkinOperatorDisassembler();
        SkinOperatorDisassembler(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& operatedSkin() const { return m_skinMeshId; }
        [[nodiscard]] const std::optional<std::string>& skinNodeName() const { return m_skinMeshNodeName; }
        [[nodiscard]] const std::vector<std::string>& bones() const { return m_boneNodeNames; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Matrix4>>& t_posOffsets() const { return m_t_posOffsets; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_skinMeshId;
        std::optional<std::string> m_skinMeshNodeName;
        std::vector<std::string> m_boneNodeNames;
        std::optional<std::vector<MathLib::Matrix4>> m_t_posOffsets;
    };
}

#endif // SKIN_OPERATOR_ASSEMBLER_H
