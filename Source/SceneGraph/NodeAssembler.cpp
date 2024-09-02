#include "NodeAssembler.h"
#include "Node.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_CHILDREN = "Children";
static std::string TOKEN_NATIVE_DTO = "NativeDto";

NodeAssembler::NodeAssembler(const SpatialId& id)
    : SpatialAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(Node::TYPE_RTTI.getName());
}

void NodeAssembler::child(const SpatialId& child_id)
{
    m_children.push_back({ child_id, nullptr });
}

void NodeAssembler::child(const std::shared_ptr<SpatialAssembler>& child)
{
    assert(child);
    m_children.push_back({ child->id(), child });
}

void NodeAssembler::child(const std::shared_ptr<Spatial>& child)
{
    assert(child);
    std::shared_ptr<SpatialAssembler> childAssembler = SpatialAssembler::assembledAssemblerOf(child);
    m_children.push_back({ child->id(), childAssembler });
}

void NodeAssembler::consistChildrenLocationBounding()
{
    if (m_children.empty()) return;
    Engine::BoundingVolume model_bound = m_modelBound;
    for (const auto& [id, child] : m_children)
    {
        if (!child) continue;
        child->parentWorldTransform(m_worldTransform);
        if (std::shared_ptr<NodeAssembler> nodeAssembler = std::dynamic_pointer_cast<NodeAssembler>(child))
        {
            nodeAssembler->consistChildrenLocationBounding();
        }
        child->mergeModelBoundingTo(&model_bound);
    }
    modelBound(model_bound);
}

Enigma::Engine::GenericDto NodeAssembler::assemble() const
{
    const_cast<NodeAssembler*>(this)->consistChildrenLocationBounding();
    Engine::GenericDto dto = SpatialAssembler::assemble();
    Engine::GenericDtoCollection dtos;
    for (const auto& [id, child] : m_children)
    {
        Engine::GenericDto child_dto;
        child_dto.addOrUpdate(TOKEN_ID, id.tokens());
        if (child)
        {
            child_dto.addOrUpdate(TOKEN_NATIVE_DTO, child->assemble());
        }
        dtos.push_back(child_dto);
    }
    dto.addOrUpdate(TOKEN_CHILDREN, dtos);
    return dto;
}

NodeDisassembler::NodeDisassembler() : SpatialDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(Node::TYPE_RTTI.getName());
}

void NodeDisassembler::disassemble(const Engine::GenericDto& dto)
{
    SpatialDisassembler::disassemble(dto);
    if (auto children = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_CHILDREN))
    {
        for (const Engine::GenericDto& child : children.value())
        {
            SpatialId id;
            if (auto v = child.tryGetValue<std::vector<std::string>>(TOKEN_ID)) id = v.value();
            if (auto v = child.tryGetValue<Engine::GenericDto>(TOKEN_NATIVE_DTO))
            {
                m_children.push_back({ id, v.value() });
            }
            else
            {
                m_children.push_back({ id, std::nullopt });
            }
        }
    }
}
