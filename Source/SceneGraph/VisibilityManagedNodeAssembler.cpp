#include "VisibilityManagedNodeAssembler.h"
#include "VisibilityManagedNode.h"

using namespace Enigma::SceneGraph;

DehydratedVisibilityManagedNodeAssembler::DehydratedVisibilityManagedNodeAssembler(const SpatialId& id) : DehydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI);
}

HydratedVisibilityManagedNodeAssembler::HydratedVisibilityManagedNodeAssembler(const SpatialId& id) : HydratedLazyNodeAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI);
}

DehydratedVisibilityManagedNodeDisassembler::DehydratedVisibilityManagedNodeDisassembler() : DehydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI);
}

HydratedVisibilityManagedNodeDisassembler::HydratedVisibilityManagedNodeDisassembler() : HydratedLazyNodeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI);
}