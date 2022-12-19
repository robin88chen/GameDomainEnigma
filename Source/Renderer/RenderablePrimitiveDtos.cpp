#include "RenderablePrimitiveDtos.h"
#include "RenderablePrimitivePolicies.h"

using namespace Enigma::Renderer;

void MeshNodeDto::SetMeshPrimitivePolicy(std::unique_ptr<MeshPrimitivePolicy> policy)
{
    m_meshPrimitive = std::move(policy);
}
