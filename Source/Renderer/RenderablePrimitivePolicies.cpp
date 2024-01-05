#include "RenderablePrimitivePolicies.h"
#include "SkinMeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

MeshPrimitivePolicy::MeshPrimitivePolicy() : RenderablePrimitivePolicy(), m_geometryFactoryDesc(Geometries::GeometryData::TYPE_RTTI.getName())
{

}

MeshPrimitivePolicy::~MeshPrimitivePolicy()
{

}

std::shared_ptr<MeshPrimitive> MeshPrimitivePolicy::createPrimitive() const
{
    return nullptr; // std::make_shared<MeshPrimitive>(m_name);
}

std::shared_ptr<MeshPrimitive> SkinMeshPrimitivePolicy::createPrimitive() const
{
    return nullptr; // std::make_shared<SkinMeshPrimitive>(m_name);
}

ModelPrimitivePolicy::ModelPrimitivePolicy() : RenderablePrimitivePolicy()
{
}

ModelPrimitivePolicy::~ModelPrimitivePolicy()
{
}