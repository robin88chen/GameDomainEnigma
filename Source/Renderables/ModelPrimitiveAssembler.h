/*****************************************************************
 * \file   ModelPrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 ******************************************************************/
#ifndef MODEL_PRIMITIVE_ASSEMBLER_H
#define MODEL_PRIMITIVE_ASSEMBLER_H

#include "Animators/AnimatorId.h"
#include "Primitives/PrimitiveAssembler.h"
#include "MeshNodeTree.h"

namespace Enigma::Renderables
{
    class MeshNodeTreeAssembler;
    class MeshNodeTreeDisassembler;

    class ModelPrimitiveAssembler : public Primitives::PrimitiveAssembler
    {
    public:
        ModelPrimitiveAssembler(const Primitives::PrimitiveId& id);
        virtual ~ModelPrimitiveAssembler() override = default;
        ModelPrimitiveAssembler(const ModelPrimitiveAssembler&) = default;
        ModelPrimitiveAssembler(ModelPrimitiveAssembler&&) = default;
        ModelPrimitiveAssembler& operator=(const ModelPrimitiveAssembler&) = default;
        ModelPrimitiveAssembler& operator=(ModelPrimitiveAssembler&&) = default;

        void meshNodeTree(const MeshNodeTree& tree);
        void modelAnimatorId(const Animators::AnimatorId& id) { m_modelAnimatorId = id; }
        virtual Engine::GenericDto assemble() const override;

    protected:
        std::shared_ptr<MeshNodeTreeAssembler> m_treeAssembler;
        std::optional<Animators::AnimatorId> m_modelAnimatorId;
    };

    class ModelPrimitiveDisassembler : public Primitives::PrimitiveDisassembler
    {
    public:
        ModelPrimitiveDisassembler();
        virtual ~ModelPrimitiveDisassembler() override = default;
        ModelPrimitiveDisassembler(const ModelPrimitiveDisassembler&) = default;
        ModelPrimitiveDisassembler(ModelPrimitiveDisassembler&&) = default;
        ModelPrimitiveDisassembler& operator=(const ModelPrimitiveDisassembler&) = default;
        ModelPrimitiveDisassembler& operator=(ModelPrimitiveDisassembler&&) = default;

        [[nodiscard]] const MeshNodeTree& meshNodeTree() const { return m_meshNodeTree; }
        [[nodiscard]] const std::optional<Animators::AnimatorId>& modelAnimatorId() const { return m_modelAnimatorId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        MeshNodeTree m_meshNodeTree;
        std::optional<Animators::AnimatorId> m_modelAnimatorId;
    };
}

#endif // MODEL_PRIMITIVE_ASSEMBLER_H
