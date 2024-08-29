#include "MeshNodeAssemblers.h"
#include "MeshNode.h"
#include "MeshNodeTree.h"

using namespace Enigma::Renderables;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_LOCAL_T_POS_TRANSFORM = "LocalT_PosTransform";
static std::string TOKEN_MESH_ID = "MeshId";
static std::string TOKEN_PARENT_NODE_INDEX = "ParentNodeIndex";
static std::string TOKEN_MESH_NODES = "MeshNodes";

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

MeshNodeTreeAssembler::MeshNodeTreeAssembler() : m_factoryDesc(MeshNodeTree::TYPE_RTTI)
{
}

void MeshNodeTreeAssembler::addNodeWithParentName(const MeshNode& node, const std::string& parent_name)
{
    if (m_nodeAssemblers.empty())
    {
        auto node_assembler = std::make_shared<MeshNodeAssembler>();
        node.assemble(node_assembler);
        m_nodeNames.emplace_back(node.getName());
        m_nodeAssemblers.emplace_back(node_assembler);
    }
    else
    {
        auto node_assembler = std::make_shared<MeshNodeAssembler>();
        node.assemble(node_assembler);
        if (!parent_name.empty())
        {
            if (auto parent_index = findNodeIndex(parent_name)) node_assembler->parentIndexInArray(parent_index.value());
        }
        m_nodeNames.emplace_back(node.getName());
        m_nodeAssemblers.emplace_back(node_assembler);
    }
}

void MeshNodeTreeAssembler::addNodeWithParentName(const std::string& node_name, const std::shared_ptr<MeshNodeAssembler>& assembler, const std::string& parent_name)
{
    assert(!m_nodeAssemblers.empty()); // Must have at least one node, to add a parent
    if (!parent_name.empty())
    {
        if (auto parent_index = findNodeIndex(parent_name)) assembler->parentIndexInArray(parent_index.value());
    }
    m_nodeNames.emplace_back(node_name);
    m_nodeAssemblers.emplace_back(assembler);
}

void MeshNodeTreeAssembler::addNode(const MeshNode& node)
{
    auto node_assembler = std::make_shared<MeshNodeAssembler>();
    node.assemble(node_assembler);
    m_nodeNames.emplace_back(node.getName());
    m_nodeAssemblers.emplace_back(node_assembler);
}

void MeshNodeTreeAssembler::addNode(const std::string& node_name, const std::shared_ptr<MeshNodeAssembler>& assembler)
{
    m_nodeNames.emplace_back(node_name);
    m_nodeAssemblers.emplace_back(assembler);
}

Enigma::Engine::GenericDto MeshNodeTreeAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    std::vector<Engine::GenericDto> node_dtos;
    for (const auto& node : m_nodeAssemblers)
    {
        node_dtos.emplace_back(node->assemble());
    }
    dto.addOrUpdate(TOKEN_MESH_NODES, node_dtos);
    return dto;
}

std::optional<unsigned int> MeshNodeTreeAssembler::findNodeIndex(const std::string& name) const
{
    if (name.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_nodeNames.size(); i++)
    {
        if (m_nodeNames[i] == name)
        {
            return i;
        }
    }
    return std::nullopt;
}

MeshNodeTreeDisassembler::MeshNodeTreeDisassembler() : m_factoryDesc(MeshNodeTree::TYPE_RTTI)
{
}

void MeshNodeTreeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_MESH_NODES))
    {
        for (const auto& node_dto : v.value())
        {
            std::shared_ptr<MeshNodeDisassembler> disassembler = std::make_shared<MeshNodeDisassembler>();
            disassembler->disassemble(node_dto);
            MeshNode node;
            node.disassemble(disassembler);
            m_nodes.emplace_back(node);
        }
    }
}
