#include "MeshPrimitive.h"
#include "MeshPrimitiveAssembler.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialSource.h"
#include "GameEngine/Texture.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GameEngine/RenderBuffer.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/IRenderer.h"
#include "Renderer/RenderElement.h"
#include "Platforms/PlatformLayer.h"
#include "RenderablePrimitiveDtos.h"
#include "Geometries/GeometryDataQueries.h"
#include "RenderableErrors.h"
#include "Geometries/GeometryAssembler.h"
#include <cassert>

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;

DEFINE_RTTI(Renderables, MeshPrimitive, Primitive);

MeshPrimitive::MeshPrimitive(const Primitives::PrimitiveId& id) : Primitive(id)
{
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ready);
    m_factoryDesc = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName());
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
    m_renderListID = Renderer::Renderer::RenderListID::Scene;
    m_elements.clear();
    m_effects.clear();
    m_textures.clear();
}

/*MeshPrimitive::MeshPrimitive(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository) : Primitive(id)
{
    assert(geometry_repository);
    MeshPrimitiveDto mesh_dto(dto);
    m_factoryDesc = mesh_dto.factoryDesc();
    m_name = id.name();
    m_geometry = nullptr;
    if (geometry_repository->hasGeometryData(mesh_dto.geometryId()))
    {
        m_geometry = geometry_repository->queryGeometryData(mesh_dto.geometryId());
    }
    else if (mesh_dto.geometry())
    {
        m_geometry = std::make_shared<Geometries::RequestGeometryConstitution>(mesh_dto.geometryId(), mesh_dto.geometry().value())->dispatch();
    }
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
    m_renderBuffer = nullptr;
    m_renderListID = mesh_dto.renderListID();
    m_elements.clear();
    m_effects.clear();
    for (auto& eff_id : mesh_dto.effects())
    {
        m_effects.emplace_back(EffectMaterial::queryEffectMaterial(eff_id));
    }
    m_textures.clear();
    for (auto& tex_map_dto : mesh_dto.textureMaps())
    {
        EffectTextureMap tex_map;
        std::shared_ptr<EffectTextureMapDisassembler> disassembler = std::make_shared<EffectTextureMapDisassembler>();
        disassembler->disassemble(tex_map_dto);
        tex_map.disassemble(disassembler);
        m_textures.emplace_back(tex_map);
    }
}*/

MeshPrimitive::~MeshPrimitive()
{
    cleanupGeometry();
    loosePrimitiveEffectTexture();
    m_effects.clear();
    m_textures.clear();
}

std::shared_ptr<Enigma::Primitives::PrimitiveAssembler> MeshPrimitive::assembler() const
{
    return std::make_shared<MeshPrimitiveAssembler>(m_id.origin());
}

void MeshPrimitive::assemble(const std::shared_ptr<Primitives::PrimitiveAssembler>& assembler) const
{
    assembler->factoryDesc(m_factoryDesc);
    const auto mesh_assembler = std::dynamic_pointer_cast<MeshPrimitiveAssembler, Primitives::PrimitiveAssembler>(assembler);
    if (!mesh_assembler) return;
    if (m_geometry)
    {
        mesh_assembler->geometryId(m_geometry->id());
        if ((m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Native)
            || (m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::ResourceAsset))
        {
            mesh_assembler->geometry(m_geometry);
        }
    }
    for (auto& eff : m_effects)
    {
        mesh_assembler->addEffect(eff->id());
    }
    for (auto& tex : m_textures)
    {
        if (!tex.isAllResourceTexture()) continue;
        std::shared_ptr<EffectTextureMapAssembler> tex_assembler = std::make_shared<EffectTextureMapAssembler>();
        tex.assemble(tex_assembler);
        mesh_assembler->addTextureMap(tex_assembler);
    }
    mesh_assembler->renderListID(m_renderListID);
    mesh_assembler->visualTechnique(m_selectedVisualTech);
}

std::shared_ptr<Enigma::Primitives::PrimitiveDisassembler> MeshPrimitive::disassembler() const
{
    return std::make_shared<MeshPrimitiveDisassembler>();
}

void MeshPrimitive::disassemble(const std::shared_ptr<Primitives::PrimitiveDisassembler>& disassembler)
{
    assert(disassembler);
    m_id = disassembler->id();
    m_factoryDesc = disassembler->factoryDesc();
    const auto mesh_disassembler = std::dynamic_pointer_cast<MeshPrimitiveDisassembler, Primitives::PrimitiveDisassembler>(disassembler);
    m_geometry = nullptr;
    if (std::make_shared<Geometries::HasGeometryData>(mesh_disassembler->geometryId())->dispatch())
    {
        m_geometry = std::make_shared<Geometries::QueryGeometryData>(mesh_disassembler->geometryId())->dispatch();
    }
    else if (mesh_disassembler->rawGeometryDto().has_value())
    {
        m_geometry = std::make_shared<Geometries::RequestGeometryConstitution>(mesh_disassembler->geometryId(), mesh_disassembler->rawGeometryDto().value())->dispatch();
    }
    m_lazyStatus.changeStatus(Frameworks::LazyStatus::Status::Ghost);
    m_renderBuffer = nullptr;
    m_elements.clear();
    m_effects.clear();
    for (auto& eff_id : mesh_disassembler->effects())
    {
        m_effects.emplace_back(EffectMaterial::queryEffectMaterial(eff_id));
    }
    m_textures.clear();
    for (auto& tex_map_dto : mesh_disassembler->textureMaps())
    {
        EffectTextureMap tex_map;
        tex_map.disassemble(tex_map_dto);
        m_textures.emplace_back(tex_map);
    }
    m_renderListID = mesh_disassembler->renderListID();
    m_selectedVisualTech = mesh_disassembler->visualTechniqueSelection();
}

/*GenericDto MeshPrimitive::serializeDto() const
{
    return serializeMeshDto().toGenericDto();
}*/

/*MeshPrimitiveDto MeshPrimitive::serializeMeshDto() const
{
    MeshPrimitiveDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.id() = m_id.origin();
    if (m_geometry)
    {
        dto.geometryId() = m_geometry->id();
        if ((m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Native)
            || (m_geometry->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::ResourceAsset))
        {
            std::shared_ptr<Geometries::GeometryAssembler> assembler = m_geometry->assembler();
            m_geometry->assemble(assembler);
            dto.geometry() = assembler->assemble();
        }
    }
    for (auto& eff : m_effects)
    {
        dto.effects().emplace_back(eff->id());
    }
    for (auto& tex : m_textures)
    {
        if (!tex.isAllResourceTexture()) continue;
        std::shared_ptr<EffectTextureMapAssembler> tex_assembler = std::make_shared<EffectTextureMapAssembler>();
        tex.assemble(tex_assembler);
        dto.textureMaps().emplace_back(tex_assembler->assemble());
    }
    dto.renderListID() = m_renderListID;
    dto.visualTechniqueSelection() = m_selectedVisualTech;
    return dto;
}*/

std::shared_ptr<EffectMaterial> MeshPrimitive::getEffectMaterial(unsigned index)
{
    if (index >= m_effects.size()) return nullptr;
    return m_effects[index];
}

unsigned MeshPrimitive::getEffectMaterialCount() const
{
    return static_cast<unsigned>(m_effects.size());
}

const EffectTextureMap& MeshPrimitive::getTextureMap(unsigned index)
{
    assert(index < m_textures.size());
    return m_textures[index];
}

unsigned MeshPrimitive::getTextureMapCount() const
{
    return static_cast<unsigned>(m_textures.size());
}

void MeshPrimitive::changeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    if (m_textures.empty()) return;
    loosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        tm.changeSemanticTexture(tuple);
    }
    bindPrimitiveEffectTexture();
}

void MeshPrimitive::bindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    if (m_textures.empty()) return;
    loosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        tm.bindSemanticTexture(tuple);
    }
    bindPrimitiveEffectTexture();
}

void MeshPrimitive::bindSemanticTextures(const EffectTextureMap::SegmentEffectTextures& texture_tuples)
{
    if (m_textures.empty()) return;
    loosePrimitiveEffectTexture();
    for (auto& tm : m_textures)
    {
        for (auto& tuple : texture_tuples)
        {
            tm.bindSemanticTexture(tuple);
        }
    }
    bindPrimitiveEffectTexture();
}

error MeshPrimitive::updateRenderBuffer()
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    const error er = m_renderBuffer->updateVertex(m_geometry->getVertexMemory(), m_geometry->getIndexMemory());
    return er;
}

error MeshPrimitive::rangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count,
    std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count)
{
    assert(m_geometry);
    if (!m_renderBuffer) return ErrorCode::nullRenderBuffer;
    std::optional<IIndexBuffer::ranged_buffer> idx_memory;
    if (idx_count && idx_offset) idx_memory = m_geometry->getRangedIndexMemory(idx_offset.value(), idx_count.value());
    const error er = m_renderBuffer->rangedUpdateVertex(m_geometry->getRangedVertexMemory(vtx_offset, vtx_count), idx_memory);
    return er;
}

error MeshPrimitive::insertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
    const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState)
{
    if (!m_lazyStatus.isReady()) return ErrorCode::ok;
    const auto render = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    m_mxPrimitiveWorld = mxWorld;
    if (testPrimitiveFlag(Primitive_UnRenderable)) return ErrorCode::ok;

    if (FATAL_LOG_EXPR(m_elements.empty())) return ErrorCode::emptyRenderElementList;

    error er = ErrorCode::ok;
    for (auto& ele : m_elements)
    {
        er = render->insertRenderElement(ele, mxWorld, lightingState, m_renderListID);
        if (er) return er;
    }
    return er;
}

error MeshPrimitive::removeFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer)
{
    const auto render = std::dynamic_pointer_cast<Renderer::Renderer, Engine::IRenderer>(renderer);
    if (FATAL_LOG_EXPR(!render)) return ErrorCode::nullRenderer;
    if (FATAL_LOG_EXPR(m_elements.empty())) return ErrorCode::emptyRenderElementList;
    for (auto& ele : m_elements)
    {
        render->removeRenderElement(ele, m_renderListID);
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

void MeshPrimitive::updateWorldTransform(const MathLib::Matrix4& mxWorld)
{
    m_mxPrimitiveWorld = mxWorld;
}

void MeshPrimitive::selectVisualTechnique(const std::string& techniqueName)
{
    Primitive::selectVisualTechnique(techniqueName);
    for (auto& eff : m_effects)
    {
        eff->selectVisualTechnique(techniqueName);
    }
}

void MeshPrimitive::linkGeometryData(const Geometries::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer)
{
    cleanupGeometry();
    m_geometry = geo;
    m_renderBuffer = render_buffer;
    m_bound = m_geometry->getBoundingVolume();
}

void MeshPrimitive::changeEffectMaterialInSegment(unsigned index, const std::shared_ptr<EffectMaterial>& effect)
{
    if (index >= m_effects.size()) return;
    looseSegmentEffectTexture(index);
    m_effects[index] = effect;
    bindSegmentEffectTexture(index);
}

void MeshPrimitive::changeEffectMaterials(const EffectMaterialList& effects)
{
    loosePrimitiveEffectTexture();
    m_effects.clear();
    if (effects.size() == 0) return;
    m_effects = effects;
    bindPrimitiveEffectTexture();
}

void MeshPrimitive::changeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map)
{
    if (index >= m_textures.size()) return;
    looseSegmentEffectTexture(index);
    m_textures[index] = tex_map;
    bindSegmentEffectTexture(index);
}

void MeshPrimitive::changeTextureMaps(const TextureMapList& tex_maps)
{
    loosePrimitiveEffectTexture();
    m_textures.clear();
    if (tex_maps.size() == 0) return;
    m_textures = tex_maps;
    bindPrimitiveEffectTexture();
}

void MeshPrimitive::createRenderElements()
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
        m_elements.emplace_back(std::make_shared<Renderer::RenderElement>(m_renderBuffer, m_effects[i], m_geometry->getSegment(i)));
    }
}

void MeshPrimitive::cleanupGeometry()
{
    m_elements.clear();
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
}

void MeshPrimitive::bindPrimitiveEffectTexture()
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
        for (unsigned i = 0; i < (*tex_iter).getCount(); i++)
        {
            auto& eff_tex_set = (*tex_iter).getEffectSemanticTextureTuple(i);
            if (std::get<std::shared_ptr<Texture>>(eff_tex_set) == nullptr) continue;
            // 改直接指定
            (*eff_iter)->assignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{
                        std::get<std::shared_ptr<Texture>>(eff_tex_set)->getDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto& var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{
                        std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
                });*/
        }
    }
}

void MeshPrimitive::loosePrimitiveEffectTexture()
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
        for (unsigned i = 0; i < (*tex_iter).getCount(); i++)
        {
            auto& eff_tex_set = (*tex_iter).getEffectSemanticTextureTuple(i);
            // 改直接指定
            (*eff_iter)->assignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
                [=](auto& var)
                {
                    var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
                });*/
        }
    }
}

void MeshPrimitive::bindSegmentEffectTexture(unsigned index)
{
    if (index >= m_effects.size()) return;
    if (index >= m_textures.size()) return;
    if (m_effects[index] == nullptr) return;
    for (unsigned i = 0; i < m_textures[index].getCount(); i++)
    {
        auto& eff_tex_set = (m_textures[index]).getEffectSemanticTextureTuple(i);
        if (std::get<std::shared_ptr<Texture>>(eff_tex_set) == nullptr) continue;
        // 改直接指定
        m_effects[index]->assignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{
                    std::get<std::shared_ptr<Texture>>(eff_tex_set)->getDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
        /*m_effects[index]->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            });*/
    }
}

void MeshPrimitive::looseSegmentEffectTexture(unsigned index)
{
    if (index >= m_effects.size()) return;
    if (index >= m_textures.size()) return;
    if (m_effects[index] == nullptr) return;
    for (unsigned i = 0; i < m_textures[index].getCount(); i++)
    {
        auto& eff_tex_set = m_textures[index].getEffectSemanticTextureTuple(i);
        // 改直接指定
        m_effects[index]->assignVariableValue(std::get<std::string>(eff_tex_set), IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
        /*m_effects[index]->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            });*/
    }
}

std::shared_ptr<Texture> MeshPrimitive::findTextureBySemantic(const std::string& semantic) const
{
    for (auto& eff_tex : m_textures)
    {
        auto tex_tuple = eff_tex.findSemanticTexture(semantic);
        if (!tex_tuple) continue;
        if (const auto& tex = std::get<std::shared_ptr<Texture>>(tex_tuple.value()))
        {
            return tex;
        }
    }
    return nullptr;
}
