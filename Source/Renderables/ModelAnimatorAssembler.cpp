#include "ModelAnimatorAssembler.h"
#include "ModelPrimitiveAnimator.h"
#include "SkinOperatorAssembler.h"
#include "Animators/AnimatorQueries.h"
#include "GameEngine/GenericDto.h"

using namespace Enigma::Renderables;
using namespace Enigma::Animators;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_CONTROLLED_PRIMITIVE_ID = "ControlledPrimitiveId";
static std::string TOKEN_ASSET_ID = "AssetId";
static std::string TOKEN_SKIN_OPERATORS = "SkinOperators";

ModelAnimatorAssembler::ModelAnimatorAssembler(const Animators::AnimatorId& id) : AnimatorAssembler(id), m_factoryDesc(ModelPrimitiveAnimator::TYPE_RTTI)
{
}

void ModelAnimatorAssembler::addSkinOperator(const SkinAnimationOperator& skin_operator)
{
    m_skinOperators.emplace_back(skin_operator);
}

void ModelAnimatorAssembler::asNative(const std::string& file_at_path)
{
    m_factoryDesc.ClaimAsNative(file_at_path);
}

Enigma::Engine::GenericDto ModelAnimatorAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addRtti(m_factoryDesc);
    if (m_controlledPrimitiveId) dto.addOrUpdate(TOKEN_CONTROLLED_PRIMITIVE_ID, m_controlledPrimitiveId.value().tokens());
    if (m_animationAssetId) dto.addOrUpdate(TOKEN_ASSET_ID, m_animationAssetId.value().name());
    if (!m_skinOperators.empty())
    {
        Engine::GenericDtoCollection skin_operators;
        for (const auto& skin_operator : m_skinOperators)
        {
            std::shared_ptr<SkinOperatorAssembler> skin_operator_assembler = std::make_shared<SkinOperatorAssembler>();
            skin_operator.assemble(skin_operator_assembler);
            skin_operators.emplace_back(skin_operator_assembler->assemble());
        }
        dto.addOrUpdate(TOKEN_SKIN_OPERATORS, skin_operators);
    }
    return dto;
}

ModelAnimatorDisassembler::ModelAnimatorDisassembler() : m_factoryDesc(ModelPrimitiveAnimator::TYPE_RTTI)
{
}

void ModelAnimatorDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) m_id = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_CONTROLLED_PRIMITIVE_ID)) m_controlledPrimitiveId = v.value();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ASSET_ID)) m_animationAssetId = v.value();
    if (auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_SKIN_OPERATORS))
    {
        for (const auto& skin_operator_dto : v.value())
        {
            std::shared_ptr<SkinOperatorDisassembler> skin_operator_disassembler = std::make_shared<SkinOperatorDisassembler>(skin_operator_dto);
            SkinAnimationOperator skin_operator;
            skin_operator.disassemble(skin_operator_disassembler);
            m_skinOperators.emplace_back(skin_operator);
        }
    }
}

/*SkinOperatorAssembler::SkinOperatorAssembler()
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
}*/

