#include "RenderablePrimitivePolicies.h"
#include <cassert>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

MeshPrimitivePolicy::MeshPrimitivePolicy() : RenderablePrimitivePolicy(), m_geometryFactoryDesc(GeometryData::TYPE_RTTI.GetName())
{

}

MeshPrimitivePolicy::~MeshPrimitivePolicy()
{

}

/*const EffectTextureMapPolicy::TextureTuplePolicy& MeshPrimitivePolicy::GetTextureTuplePolicy(
    unsigned tex_idx, unsigned tuple_idx) const
{
    assert(tex_idx < m_texturePolicies.size());
    assert(tuple_idx < m_texturePolicies[tex_idx].GetTuplePolicies().size());
    return m_texturePolicies[tex_idx].GetTuplePolicies()[tuple_idx];
}*/

ModelPrimitivePolicy::ModelPrimitivePolicy() : RenderablePrimitivePolicy()
{
}

ModelPrimitivePolicy::~ModelPrimitivePolicy()
{
}