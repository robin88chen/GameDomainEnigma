#include "LazyNodeCreationMethods.h"
#include "SpatialAssembler.h"
#include "LazyNode.h"
#include "VisibilityManagedNode.h"
#include "LazyNodeAssembler.h"
#include <cassert>

using namespace Enigma::SceneGraph;

std::shared_ptr<LazyNode> LazyNodeCreationMethod::create(const SpatialId& id)
{
    return std::make_shared<LazyNode>(id);
}

std::shared_ptr<LazyNode> LazyNodeCreationMethod::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    auto lazy_node = std::make_shared<LazyNode>(id);
    SpatialDisassembler::disassemble(lazy_node, dto);
    return lazy_node;
}

std::error_code LazyNodeCreationMethod::hydrate(const std::shared_ptr<LazyNode>& lazy_node, const Engine::GenericDto& dto)
{
    assert(lazy_node);
    std::shared_ptr<HydratedLazyNodeDisassembler> disassembler = lazy_node->disassemblerOfLaziedContent();
    assert(disassembler);
    disassembler->disassemble(dto);
    return lazy_node->hydrate(disassembler);
}

std::shared_ptr<VisibilityManagedNode> VisibilityManagedNodeCreationMethod::create(const SpatialId& id)
{
    return std::make_shared<VisibilityManagedNode>(id);
}

std::shared_ptr<VisibilityManagedNode> VisibilityManagedNodeCreationMethod::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    auto visibility_managed_node = std::make_shared<VisibilityManagedNode>(id);
    SpatialDisassembler::disassemble(visibility_managed_node, dto);
    return visibility_managed_node;
}
