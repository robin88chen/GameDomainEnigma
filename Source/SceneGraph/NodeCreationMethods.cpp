#include "NodeCreationMethods.h"
#include "Node.h"
#include "SpatialAssembler.h"

using namespace Enigma::SceneGraph;

std::shared_ptr<Node> NodeCreationMethod::create(const SpatialId& id)
{
    return std::make_shared<Node>(id);
}

std::shared_ptr<Node> NodeCreationMethod::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    auto node = std::make_shared<Node>(id);
    SpatialDisassembler::disassemble(node, dto);
    return node;
}

