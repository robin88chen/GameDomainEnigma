/*****************************************************************
 * \file   ModelAnimatorAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 ******************************************************************/
#ifndef MODEL_ANIMATOR_ASSEMBLER_H
#define MODEL_ANIMATOR_ASSEMBLER_H

#include "Animators/AnimatorId.h"
#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorAssembler.h"
#include "Primitives/PrimitiveId.h"
#include "SkinAnimationOperator.h"

namespace Enigma::Renderables
{
    class ModelAnimatorAssembler : public Animators::AnimatorAssembler
    {
    public:
        ModelAnimatorAssembler(const Animators::AnimatorId& id);

        void controlledPrimitive(const Primitives::PrimitiveId& id) { m_controlledPrimitiveId = id; }
        void animationAsset(const Animators::AnimationAssetId& id) { m_animationAssetId = id; }
        void addSkinOperator(const SkinAnimationOperator& skin_operator);
        void asNative(const std::string& file_at_path);

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<Primitives::PrimitiveId> m_controlledPrimitiveId;
        std::optional<Animators::AnimationAssetId> m_animationAssetId;
        std::vector<SkinAnimationOperator> m_skinOperators;
    };

    class ModelAnimatorDisassembler : public Animators::AnimatorDisassembler
    {
    public:
        ModelAnimatorDisassembler();

        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& controlledPrimitiveId() const { return m_controlledPrimitiveId; }
        [[nodiscard]] const std::optional<Animators::AnimationAssetId>& animationAssetId() const { return m_animationAssetId; }
        [[nodiscard]] const std::vector<SkinAnimationOperator>& skinOperators() const { return m_skinOperators; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<Primitives::PrimitiveId> m_controlledPrimitiveId;
        std::optional<Animators::AnimationAssetId> m_animationAssetId;
        std::vector<SkinAnimationOperator> m_skinOperators;
    };
    /*class SkinOperatorAssembler
    {
    public:
        SkinOperatorAssembler();

        SkinOperatorAssembler& operatedSkin(const Primitives::PrimitiveId& id);
        SkinOperatorAssembler& skinNodeName(const std::string& name);
        SkinOperatorAssembler& bone(const std::string& name, const MathLib::Matrix4& t_pos_offset);
        SkinOperatorAssembler& bone(const std::string& name);
        SkinOperatorAssembler& bones(const std::vector<std::string>& names, const std::vector<MathLib::Matrix4>& t_pos_offsets);
        SkinOperatorAssembler& bones(const std::vector<std::string>& names);

        Engine::GenericDto toGenericDto();

    protected:
        SkinOperatorDto m_dto;
        std::vector<std::string> m_bones;
        std::vector<MathLib::Matrix4> m_t_posOffsets;
    };*/
}

#endif // MODEL_ANIMATOR_ASSEMBLER_H
