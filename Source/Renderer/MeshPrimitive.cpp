#include "MeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderer, MeshPrimitive, Primitive);

MeshPrimitive::MeshPrimitive(const std::string& name) : Primitive()
{
    m_name = name;
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_elements.clear();
}

MeshPrimitive::MeshPrimitive(const MeshPrimitive& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_elements = mesh.m_elements;
}

MeshPrimitive::MeshPrimitive(MeshPrimitive&& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_elements = std::move(mesh.m_elements);
}

MeshPrimitive::~MeshPrimitive()
{
    CleanupGeometry();
}

MeshPrimitive& MeshPrimitive::operator=(const MeshPrimitive& mesh)
{
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_elements = mesh.m_elements;
    return *this;
}

MeshPrimitive& MeshPrimitive::operator=(MeshPrimitive&& mesh)
{
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_elements = std::move(mesh.m_elements);
    return *this;
}

EffectMaterialPtr MeshPrimitive::GetEffectMaterial(unsigned index)
{
    if (index >= m_effects.size()) return nullptr;
    return m_effects[index];
}

unsigned MeshPrimitive::GetEffectMaterialCount() const
{
    return static_cast<unsigned>(m_effects.size());
}

void MeshPrimitive::LinkGeometryData(const Engine::GeometryDataPtr& geo)
{
    CleanupGeometry();
    m_geometry = geo;
    m_bound = m_geometry->GetBoundingVolume();
}

void MeshPrimitive::ChangeEffectMaterialInSegment(unsigned index, const Engine::EffectMaterialPtr& effect)
{
    if (index >= m_effects.size()) return;
    LooseSegmentEffectTexture(index);
    m_effects[index] = effect;
    BindSegmentEffectTexture(index);
}

void MeshPrimitive::ChangeEffectMaterial(const EffectMaterialList& effects)
{
    LoosePrimitiveEffectTexture();
    m_effects.clear();
    if (effects.size() == 0) return;
    m_effects = effects;
    BindPrimitiveEffectTexture();
}

void MeshPrimitive::CleanupGeometry()
{
    m_elements.clear();
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
}
