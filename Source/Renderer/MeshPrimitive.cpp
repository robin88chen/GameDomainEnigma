#include "MeshPrimitive.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialSource.h"
#include "GameEngine/Texture.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GameEngine/RenderBuffer.h"
#include "RendererErrors.h"
#include "GameEngine/GeometryData.h"
#include <cassert>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;

DEFINE_RTTI(Renderer, MeshPrimitive, Primitive);

MeshPrimitive::MeshPrimitive(const std::string& name) : Primitive()
{
    m_name = name;
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_elements.clear();
    m_effects.clear();
    m_textures.clear();
}

MeshPrimitive::MeshPrimitive(const MeshPrimitive& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    //todo: 這邊要改，element 不能複製，裡面的 effect 是不能共用的, elements 由 builder 來建立
    //m_elements = mesh.m_elements;
    for (auto& eff : mesh.m_effects)
    {
        m_effects.emplace_back(eff->GetEffectMaterialSource()->CloneEffectMaterial());
    }
    m_textures = mesh.m_textures;
}

MeshPrimitive::MeshPrimitive(MeshPrimitive&& mesh) : Primitive()
{
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_elements = std::move(mesh.m_elements);
    m_effects = std::move(mesh.m_effects);
    m_textures = std::move(mesh.m_textures);
}

MeshPrimitive::~MeshPrimitive()
{
    CleanupGeometry();
    LoosePrimitiveEffectTexture();
    m_effects.clear();
    m_textures.clear();
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

const EffectTextureMap& MeshPrimitive::GetTextureMap(unsigned index)
{
    assert(index < m_textures.size());
    return m_textures[index];
}

unsigned MeshPrimitive::GetTextureMapCount() const
{
    return static_cast<unsigned>(m_textures.size());
}

error MeshPrimitive::UpdateRenderBuffer()
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    error er = m_renderBuffer->UpdateVertex(m_geometry->GetVertexMemory(), m_geometry->GetIndexMemory());
    return er;
}

error MeshPrimitive::RangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count,
    std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count)
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    std::optional<IIndexBuffer::ranged_buffer> idx_memory;
    if (idx_count && idx_offset) idx_memory = m_geometry->GetRangedIndexMemory(idx_offset.value(), idx_count.value());
    error er = m_renderBuffer->RangedUpdateVertex(m_geometry->GetRangedVertexMemory(vtx_offset, vtx_count), idx_memory);
    return er;
}

void MeshPrimitive::LinkGeometryData(const Engine::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer)
{
    CleanupGeometry();
    m_geometry = geo;
    m_renderBuffer = render_buffer;
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

void MeshPrimitive::ChangeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map)
{
    if (index >= m_textures.size()) return;
    LooseSegmentEffectTexture(index);
    m_textures[index] = tex_map;
    BindSegmentEffectTexture(index);
}

void MeshPrimitive::ChangeTextureMap(const TextureMapList& tex_maps)
{
    LoosePrimitiveEffectTexture();
    m_textures.clear();
    if (tex_maps.size() == 0) return;
    m_textures = tex_maps;
    BindPrimitiveEffectTexture();
}

void MeshPrimitive::CleanupGeometry()
{
    m_elements.clear();
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
}

void MeshPrimitive::BindPrimitiveEffectTexture()
{
    if (m_effects.empty()) return;
    if (m_textures.empty()) return;
    EffectMaterialList::iterator eff_iter;
    TextureMapList::iterator tex_iter;
    for (eff_iter = m_effects.begin(), tex_iter = m_textures.begin();
        ((eff_iter != m_effects.end()) && (tex_iter != m_textures.end()));
        ++eff_iter, ++tex_iter)
    {
        if (*eff_iter == nullptr) continue;
        for (unsigned i = 0; i < (*tex_iter).GetCount(); i++)
        {
            auto& eff_tex_set = (*tex_iter).GetEffectTextureTuple(i);
            if (std::get<TexturePtr>(eff_tex_set) == nullptr) continue;
            (*eff_iter)->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{
                        std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
                });
        }
    }
}

void MeshPrimitive::LoosePrimitiveEffectTexture()
{
    if (m_effects.empty()) return;
    if (m_textures.empty()) return;
    EffectMaterialList::iterator eff_iter;
    TextureMapList::iterator tex_iter;
    for (eff_iter = m_effects.begin(), tex_iter = m_textures.begin();
        ((eff_iter != m_effects.end()) && (tex_iter != m_textures.end()));
        ++eff_iter, ++tex_iter)
    {
        if (*eff_iter == nullptr) continue;
        for (unsigned i = 0; i < (*tex_iter).GetCount(); i++)
        {
            auto& eff_tex_set = (*tex_iter).GetEffectTextureTuple(i);
            (*eff_iter)->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
                });
        }
    }
}

void MeshPrimitive::BindSegmentEffectTexture(unsigned index)
{
    if (index >= m_effects.size()) return;
    if (index >= m_textures.size()) return;
    if (m_effects[index] == nullptr) return;
    for (unsigned i = 0; i < m_textures[index].GetCount(); i++)
    {
        auto& eff_tex_set = (m_textures[index]).GetEffectTextureTuple(i);
        if (std::get<TexturePtr>(eff_tex_set) == nullptr) continue;
        m_effects[index]->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            });
    }
}

void MeshPrimitive::LooseSegmentEffectTexture(unsigned index)
{
    if (index >= m_effects.size()) return;
    if (index >= m_textures.size()) return;
    if (m_effects[index] == nullptr) return;
    for (unsigned i = 0; i < m_textures[index].GetCount(); i++)
    {
        auto& eff_tex_set = m_textures[index].GetEffectTextureTuple(i);
        m_effects[index]->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            });
    }
}
