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

ModelAnimatorAssembler::ModelAnimatorAssembler(const Animators::AnimatorId& id) : AnimatorAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI);
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

ModelAnimatorDisassembler::ModelAnimatorDisassembler() : AnimatorDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI);
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

