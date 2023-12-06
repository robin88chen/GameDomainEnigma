#include "MeshPrimitive.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialSource.h"
#include "GameEngine/Texture.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GameEngine/RenderBuffer.h"
#include "RendererErrors.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/IRenderer.h"
#include "RenderElement.h"
#include "Platforms/PlatformLayer.h"
#include "RenderablePrimitiveDtos.h"
#include <cassert>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;

DEFINE_RTTI(Renderer, MeshPrimitive, Primitive);

MeshPrimitive::MeshPrimitive(const std::string& name) : Primitive()
{
    m_factoryDesc = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName());
    m_name = name;
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_renderListID = Renderer::RenderListID::Scene;
    m_elements.clear();
    m_effects.clear();
    m_textures.clear();
}

MeshPrimitive::MeshPrimitive(const MeshPrimitive& mesh) : Primitive()
{
    m_factoryDesc = mesh.m_factoryDesc;
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_mxPrimitiveWorld = mesh.m_mxPrimitiveWorld;
    m_primitiveFlags = mesh.m_primitiveFlags;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_renderListID = mesh.m_renderListID;
    for (auto& eff : mesh.m_effects)
    {
        m_effects.emplace_back(eff->GetEffectMaterialSource()->CloneEffectMaterial());
    }
    m_textures = mesh.m_textures;
    CreateRenderElements();
    MeshPrimitive::SelectVisualTechnique(mesh.m_selectedVisualTech);
}

MeshPrimitive::MeshPrimitive(MeshPrimitive&& mesh) noexcept : Primitive()
{
    m_factoryDesc = std::move(mesh.m_factoryDesc);
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_mxPrimitiveWorld = std::move(mesh.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(mesh.m_primitiveFlags);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_renderListID = mesh.m_renderListID;
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
    if (this == &mesh) return *this;
    m_factoryDesc = mesh.m_factoryDesc;
    m_name = mesh.m_name;
    m_bound = mesh.m_bound;
    m_mxPrimitiveWorld = mesh.m_mxPrimitiveWorld;
    m_primitiveFlags = mesh.m_primitiveFlags;
    m_geometry = mesh.m_geometry;
    m_renderBuffer = mesh.m_renderBuffer;
    m_renderListID = mesh.m_renderListID;
    for (auto& eff : mesh.m_effects)
    {
        m_effects.emplace_back(eff->GetEffectMaterialSource()->CloneEffectMaterial());
    }
    m_textures = mesh.m_textures;
    CreateRenderElements();
    MeshPrimitive::SelectVisualTechnique(mesh.m_selectedVisualTech);
    return *this;
}

MeshPrimitive& MeshPrimitive::operator=(MeshPrimitive&& mesh) noexcept
{
    m_factoryDesc = std::move(mesh.m_factoryDesc);
    m_name = mesh.m_name;
    m_bound = std::move(mesh.m_bound);
    m_mxPrimitiveWorld = std::move(mesh.m_mxPrimitiveWorld);
    m_primitiveFlags = std::move(mesh.m_primitiveFlags);
    m_geometry = std::move(mesh.m_geometry);
    m_renderBuffer = std::move(mesh.m_renderBuffer);
    m_renderListID = mesh.m_renderListID;
    m_elements = std::move(mesh.m_elements);
    m_effects = std::move(mesh.m_effects);
    m_textures = std::move(mesh.m_textures);
    return *this;
}

GenericDto MeshPrimitive::serializeDto() const
{
    return SerializeMeshDto().toGenericDto();
}

MeshPrimitiveDto MeshPrimitive::SerializeMeshDto() const
{
    MeshPrimitiveDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.Name() = m_name;
    if (m_geometry)
    {
        dto.GeometryName() = m_geometry->id().name();
        dto.GeometryFactoryDesc() = m_geometry->factoryDesc();
        if ((m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Native)
            || (m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::ResourceAsset))
        {
            dto.TheGeometry() = m_geometry->serializeDto();
        }
    }
    for (auto& eff : m_effects)
    {
        dto.Effects().emplace_back(eff->serializeDto());
    }
    for (auto& tex : m_textures)
    {
        if (!tex.IsAllResourceTexture()) continue;
        dto.TextureMaps().emplace_back(tex.serializeDto());
    }
    dto.RenderListID() = m_renderListID;
    dto.VisualTechniqueSelection() = m_selectedVisualTech;
    return dto;
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

void MeshPrimitive::ChangeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    if (m_textures.empty()) return;
    LoosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        tm.ChangeSemanticTexture(tuple);
    }
    BindPrimitiveEffectTexture();
}

void MeshPrimitive::BindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    if (m_textures.empty()) return;
    LoosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        tm.BindSemanticTexture(tuple);
    }
    BindPrimitiveEffectTexture();
}

void MeshPrimitive::BindSegmentTextures(const EffectTextureMap::SegmentEffectTextures& texture_tuples)
{
    if (m_textures.empty()) return;
    LoosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        for (auto& tuple : texture_tuples)
        {
            tm.BindSemanticTexture(tuple);
        }
    }
    BindPrimitiveEffectTexture();
}

error MeshPrimitive::UpdateRenderBuffer()
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    const error er = m_renderBuffer->UpdateVertex(m_geometry->getVertexMemory(), m_geometry->getIndexMemory());
    return er;
}

error MeshPrimitive::RangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count,
    std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count)
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    std::optional<IIndexBuffer::ranged_buffer> idx_memory;
    if (idx_count && idx_offset) idx_memory = m_geometry->getRangedIndexMemory(idx_offset.value(), idx_count.value());
    const error er = m_renderBuffer->RangedUpdateVertex(m_geometry->getRangedVertexMemory(vtx_offset, vtx_count), idx_memory);
    return er;
}

error MeshPrimitive::InsertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    m_mxPrimitiveWorld = mxWorld;
    if (TestPrimitiveFlag(Primitive_UnRenderable)) return ErrorCode::ok;

    if (FATAL_LOG_EXPR(m_elements.empty())) return ErrorCode::emptyRenderElementList;

    error er = ErrorCode::ok;
    for (auto& ele : m_elements)
    {
        er = render->InsertRenderElement(ele, mxWorld, lightingState, m_renderListID);
        if (er) return er;
    }
    return er;
}

error MeshPrimitive::RemoveFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer)
{
    const auto render = std::dynamic_pointer_cast<Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    if (FATAL_LOG_EXPR(m_elements.empty())) return ErrorCode::emptyRenderElementList;
    for (auto& ele : m_elements)
    {
        render->RemoveRenderElement(ele, m_renderListID);
    }
    return ErrorCode::ok;
}

void MeshPrimitive::calculateBoundingVolume(bool axis_align)
{
    if (m_geometry)
    {
        m_geometry->calculateBoundingVolume(axis_align);
        m_bound = m_geometry->getBoundingVolume();
    }
}

void MeshPrimitive::UpdateWorldTransform(const MathLib::Matrix4& mxWorld)
{
    m_mxPrimitiveWorld = mxWorld;
}

void MeshPrimitive::SelectVisualTechnique(const std::string& techniqueName)
{
    Primitive::SelectVisualTechnique(techniqueName);
    for (auto& eff : m_effects)
    {
        eff->SelectVisualTechnique(techniqueName);
    }
}

void MeshPrimitive::LinkGeometryData(const Engine::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer)
{
    CleanupGeometry();
    m_geometry = geo;
    m_renderBuffer = render_buffer;
    m_bound = m_geometry->getBoundingVolume();
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

void MeshPrimitive::ChangeTextureMaps(const TextureMapList& tex_maps)
{
    LoosePrimitiveEffectTexture();
    m_textures.clear();
    if (tex_maps.size() == 0) return;
    m_textures = tex_maps;
    BindPrimitiveEffectTexture();
}

void MeshPrimitive::CreateRenderElements()
{
    assert(m_geometry);
    assert(m_renderBuffer);
    unsigned elem_count = m_geometry->getSegmentCount();
    if (elem_count > m_effects.size()) elem_count = static_cast<unsigned>(m_effects.size());
    assert(elem_count > 0);
    m_elements.clear();
    m_elements.reserve(elem_count);
    for (unsigned i = 0; i < elem_count; i++)
    {
        m_elements.emplace_back(std::make_shared<RenderElement>(m_renderBuffer, m_effects[i], m_geometry->getSegment(i)));
    }
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
            auto& eff_tex_set = (*tex_iter).GetEffectSemanticTextureTuple(i);
            if (std::get<TexturePtr>(eff_tex_set) == nullptr) continue;
            // 改直接指定
            (*eff_iter)->AssignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{
                        std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            /*(*eff_iter)->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto& var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{
                        std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
                });*/
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
            auto& eff_tex_set = (*tex_iter).GetEffectSemanticTextureTuple(i);
            // 改直接指定
            (*eff_iter)->AssignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            /*(*eff_iter)->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto& var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
                });*/
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
        auto& eff_tex_set = (m_textures[index]).GetEffectSemanticTextureTuple(i);
        if (std::get<TexturePtr>(eff_tex_set) == nullptr) continue;
        // 改直接指定
        m_effects[index]->AssignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
        /*m_effects[index]->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            });*/
    }
}

void MeshPrimitive::LooseSegmentEffectTexture(unsigned index)
{
    if (index >= m_effects.size()) return;
    if (index >= m_textures.size()) return;
    if (m_effects[index] == nullptr) return;
    for (unsigned i = 0; i < m_textures[index].GetCount(); i++)
    {
        auto& eff_tex_set = m_textures[index].GetEffectSemanticTextureTuple(i);
        // 改直接指定
        m_effects[index]->AssignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
        /*m_effects[index]->SetVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            });*/
    }
}

std::shared_ptr<Texture> MeshPrimitive::FindTextureBySemantic(const std::string& semantic) const
{
    for (auto& eff_tex : m_textures)
    {
        auto tex_tuple = eff_tex.FindSemanticTexture(semantic);
        if (!tex_tuple) continue;
        if (auto tex = std::get<std::shared_ptr<Texture>>(tex_tuple.value()))
        {
            return tex;
        }
    }
    return nullptr;
}
