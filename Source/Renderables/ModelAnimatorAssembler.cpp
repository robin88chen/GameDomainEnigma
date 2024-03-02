#include "ModelAnimatorAssembler.h"
#include "ModelPrimitiveAnimator.h"
#include "Animators/AnimatorQueries.h"

using namespace Enigma::Renderables;
using namespace Enigma::Animators;

ModelAnimatorAssembler::ModelAnimatorAssembler(const Animators::AnimatorId& id)
{
    m_id = id;
    m_dto.id() = id;
}

ModelAnimatorAssembler& ModelAnimatorAssembler::controlledPrimitive(const Primitives::PrimitiveId& id)
{
    m_dto.controlledPrimitiveId() = id;
    return *this;
}

ModelAnimatorAssembler& ModelAnimatorAssembler::animationAsset(const Animators::AnimationAssetId& id)
{
    m_dto.animationAssetId() = id;
    return *this;
}

ModelAnimatorAssembler& ModelAnimatorAssembler::skinOperator(const SkinOperatorAssembler& skin_operator)
{
    SkinOperatorAssembler assembler = skin_operator;
    m_dto.skinOperators().emplace_back(assembler.toGenericDto());
    return *this;
}

ModelAnimatorAssembler& ModelAnimatorAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto ModelAnimatorAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

std::shared_ptr<ModelPrimitiveAnimator> ModelAnimatorAssembler::constitute(Animators::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<ModelPrimitiveAnimator>(std::make_shared<RequestAnimatorConstitution>(m_id, toGenericDto(), persistence_level)->dispatch());
}

SkinOperatorAssembler::SkinOperatorAssembler()
{
}

SkinOperatorAssembler& SkinOperatorAssembler::operatedSkin(const Primitives::PrimitiveId& id)
{
    m_dto.skinMeshId() = id;
    return *this;
}

SkinOperatorAssembler& SkinOperatorAssembler::skinNodeName(const std::string& name)
{
    m_dto.skinMeshNodeName() = name;
    return *this;
}

SkinOperatorAssembler& SkinOperatorAssembler::bone(const std::string& name, const MathLib::Matrix4& t_pos_offset)
{
    assert(m_bones.size() == m_t_posOffsets.size());  // must have same size
    m_bones.emplace_back(name);
    m_t_posOffsets.emplace_back(t_pos_offset);
    return *this;
}

SkinOperatorAssembler& SkinOperatorAssembler::bone(const std::string& name)
{
    assert(m_t_posOffsets.empty());  // offsets must be empty
    m_bones.emplace_back(name);
    return *this;
}

SkinOperatorAssembler& SkinOperatorAssembler::bones(const std::vector<std::string>& names, const std::vector<MathLib::Matrix4>& t_pos_offsets)
{
    m_bones = names;
    m_t_posOffsets = t_pos_offsets;
    return *this;
}

SkinOperatorAssembler& SkinOperatorAssembler::bones(const std::vector<std::string>& names)
{
    m_t_posOffsets.clear();
    m_bones = names;
    return *this;
}

Enigma::Engine::GenericDto SkinOperatorAssembler::toGenericDto()
{
    m_dto.boneNodeNames() = m_bones;
    if (!m_t_posOffsets.empty())
    {
        m_dto.nodeOffsets() = m_t_posOffsets;
    }
    return m_dto.toGenericDto();
}

