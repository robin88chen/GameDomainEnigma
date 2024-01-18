#include "TerrainPrimitivePolicy.h"
#include "TerrainPrimitive.h"

using namespace Enigma::Terrain;

TerrainPrimitivePolicy::TerrainPrimitivePolicy() : MeshPrimitivePolicy()
{
}

TerrainPrimitivePolicy::TerrainPrimitivePolicy(const MeshPrimitivePolicy& policy) : MeshPrimitivePolicy(policy)
{
}

std::shared_ptr<Enigma::Renderables::MeshPrimitive> TerrainPrimitivePolicy::createPrimitive() const
{
    return nullptr; // std::make_shared<TerrainPrimitive>(m_name);
}

