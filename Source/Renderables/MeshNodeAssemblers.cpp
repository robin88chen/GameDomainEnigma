#include "MeshNodeAssemblers.h"
#include "MeshNode.h"
using namespace Enigma::Renderables;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_LOCAL_T_POS_TRANSFORM = "LocalT_PosTransform";
static std::string TOKEN_MESH_ID = "MeshId";
static std::string TOKEN_PARENT_NODE_INDEX = "ParentNodeIndex";

MeshNodeAssembler::MeshNodeAssembler()
    : m_factoryDesc(MeshNode::TYPE_RTTI)
{
}

Enigma::Engine::GenericDto MeshNodeAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_NAME, m_name);
    dto.addOrUpdate(TOKEN_LOCAL_T_POS_TRANSFORM, m_localT_PosTransform);
    //dto.addOrUpdate(TOKEN_ROOT_REF_TRANSFORM, m_rootRefTransform);
    if (m_meshPrimitiveId) dto.addOrUpdate(TOKEN_MESH_ID, m_meshPrimitiveId.value().tokens());
    if (m_parentIndexInArray)
    {
        dto.addOrUpdate(TOKEN_PARENT_NODE_INDEX, m_parentIndexInArray.value());
    }
    return dto;
}

MeshNodeDisassembler::MeshNodeDisassembler() : m_factoryDesc(MeshNode::TYPE_RTTI)
{
}

void MeshNodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_NAME)) m_name = v.value();
    if (const auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_T_POS_TRANSFORM)) m_localT_PosTransform = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_ID)) m_meshPrimitiveId = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) m_parentIndexInArray = v.value();
}

