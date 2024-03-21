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
#include "ModelAnimatorDtos.h"
#include "Primitives/PrimitiveId.h"
#include "Animators/AnimatorPersistenceLevel.h"

namespace Enigma::Renderables
{
    class SkinOperatorAssembler;
    class ModelPrimitiveAnimator;

    class ModelAnimatorAssembler
    {
    public:
        ModelAnimatorAssembler(const Animators::AnimatorId& id);

        ModelAnimatorAssembler& controlledPrimitive(const Primitives::PrimitiveId& id);
        ModelAnimatorAssembler& animationAsset(const Animators::AnimationAssetId& id);
        ModelAnimatorAssembler& skinOperator(const SkinOperatorAssembler& skin_operator);
        ModelAnimatorAssembler& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto();

        std::shared_ptr<ModelPrimitiveAnimator> constitute(Animators::PersistenceLevel persistence_level);

    protected:
        Animators::AnimatorId m_id;
        ModelAnimatorDto m_dto;
    };

    class SkinOperatorAssembler
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
    };
}

#endif // MODEL_ANIMATOR_ASSEMBLER_H
