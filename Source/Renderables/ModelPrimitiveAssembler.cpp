#include "ModelPrimitiveAssembler.h"
#include "MeshNodeAssemblers.h"
#include "ModelPrimitive.h"

using namespace Enigma::Renderables;
static std::string TOKEN_ID = "Id";
static std::string TOKEN_MESH_NODE_TREE = "MeshNodeTree";
static std::string TOKEN_MODEL_ANIMATOR_ID = "ModelAnimatorId";

ModelPrimitiveAssembler::ModelPrimitiveAssembler(const Primitives::PrimitiveId& id)
    : Primitives::PrimitiveAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(ModelPrimitive::TYPE_RTTI.getName());
}

void ModelPrimitiveAssembler::meshNodeTree(const MeshNodeTree& tree)
{
    m_treeAssembler = std::make_shared<MeshNodeTreeAssembler>();
    tree.assemble(m_treeAssembler);
}

void ModelPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_factoryDesc.ClaimAsNative(file_at_path);
}

Enigma::Engine::GenericDto ModelPrimitiveAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    if (m_treeAssembler) dto.addOrUpdate(TOKEN_MESH_NODE_TREE, m_treeAssembler->assemble());
    if (m_modelAnimatorId) dto.addOrUpdate(TOKEN_MODEL_ANIMATOR_ID, m_modelAnimatorId.value().tokens());
    return dto;
}

ModelPrimitiveDisassembler::ModelPrimitiveDisassembler() : Primitives::PrimitiveDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(ModelPrimitive::TYPE_RTTI.getName());
}

void ModelPrimitiveDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID))
    {
        m_id = Primitives::PrimitiveId(v.value());
    }
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_MESH_NODE_TREE))
    {
        std::shared_ptr<MeshNodeTreeDisassembler> tree_disassembler = std::make_shared<MeshNodeTreeDisassembler>();
        tree_disassembler->disassemble(v.value());
        m_meshNodeTree.disassemble(tree_disassembler);
    }
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MODEL_ANIMATOR_ID))
    {
        m_modelAnimatorId = Animators::AnimatorId(v.value());
    }
}