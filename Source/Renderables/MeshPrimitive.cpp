#include "MeshPrimitive.h"
#include "MeshPrimitiveAssembler.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialSource.h"
#include "GraphicKernel/IShaderVariable.h"
#include "GameEngine/RenderBuffer.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/IRenderer.h"
#include "Renderer/RenderElement.h"
#include "Platforms/PlatformLayer.h"
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
    m_materials.clear();
}

MeshPrimitive::~MeshPrimitive()
{
    cleanupGeometry();
    loosePrimitiveMaterials();
    m_materials.clear();
}

std::shared_ptr<MeshPrimitive> MeshPrimitive::create(const Primitives::PrimitiveId& id)
{
    return std::make_shared<MeshPrimitive>(id);
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
        //if ((m_geometry->factoryDesc().instanceType() == FactoryDesc::InstanceType::Native)
          //  || (m_geometry->factoryDesc().instanceType() == FactoryDesc::InstanceType::ResourceAsset))
        if (m_geometry->factoryDesc().instanceType() == FactoryDesc::InstanceType::Native) // only native geometry need to be assembled
        {
            mesh_assembler->geometry(m_geometry);
        }
    }
    for (auto& mat : m_materials)
    {
        mesh_assembler->addMaterial(mat);
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
    assert(m_id.origin() == disassembler->id().origin()); // id is already set in the constructor
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
    m_materials = mesh_disassembler->materials();
    m_renderListID = mesh_disassembler->renderListID();
    m_selectedVisualTech = mesh_disassembler->visualTechniqueSelection();
}

std::shared_ptr<PrimitiveMaterial> MeshPrimitive::getMaterial(unsigned index)
{
    if (index >= m_materials.size()) return nullptr;
    return m_materials[index];
}

unsigned MeshPrimitive::getMaterialCount() const
{
    return static_cast<unsigned>(m_materials.size());
}

void MeshPrimitive::changeMaterials(const std::vector<std::shared_ptr<PrimitiveMaterial>>& materials)
{
    loosePrimitiveMaterials();
    m_materials = materials;
    bindPrimitiveMaterials();
}

void MeshPrimitive::rebindMaterials()
{
    loosePrimitiveMaterials();
    bindPrimitiveMaterials();
}

void MeshPrimitive::changeSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture)
{
    if (m_materials.empty()) return;
    loosePrimitiveMaterials();
    for (auto& mat : m_materials)
    {
        if (mat == nullptr) continue;
        mat->changeSemanticTexture(semantic_texture);
    }
    bindPrimitiveMaterials();
}

void MeshPrimitive::bindSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture)
{
    if (m_materials.empty()) return;
    loosePrimitiveMaterials();
    for (auto& mat : m_materials)
    {
        if (mat == nullptr) continue;
        mat->bindSemanticTexture(semantic_texture);
    }
    bindPrimitiveMaterials();
}

void MeshPrimitive::bindSemanticTextures(const std::vector<EffectSemanticTexture>& textures)
{
    if (m_materials.empty()) return;
    loosePrimitiveMaterials();
    for (auto& mat : m_materials)
    {
        if (mat == nullptr) continue;
        mat->bindSemanticTextures(textures);
    }
    bindPrimitiveMaterials();
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
    for (auto mat : m_materials)
    {
        if (mat) mat->selectVisualTechnique(techniqueName);
    }
}

void MeshPrimitive::linkGeometryData(const Geometries::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer)
{
    cleanupGeometry();
    m_geometry = geo;
    m_renderBuffer = render_buffer;
    m_bound = m_geometry->getBoundingVolume();
}

void MeshPrimitive::changeEffectInSegment(unsigned index, const std::shared_ptr<EffectMaterial>& effect)
{
    if (index >= m_materials.size()) return;
    looseSegmentMaterial(index);
    if (m_materials[index]) m_materials[index]->changeEffect(effect);
    bindSegmentMaterial(index);
}

void MeshPrimitive::changeEffects(const EffectMaterialList& effects)
{
    loosePrimitiveMaterials();
    for (unsigned i = 0; i < effects.size() && i < m_materials.size(); i++)
    {
        if (m_materials[i]) m_materials[i]->changeEffect(effects[i]);
    }
    bindPrimitiveMaterials();
}

void MeshPrimitive::changeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map)
{
    if (index >= m_materials.size()) return;
    looseSegmentMaterial(index);
    if (m_materials[index]) m_materials[index]->changeTextureMap(tex_map);
    bindSegmentMaterial(index);
}

void MeshPrimitive::changeTextureMaps(const TextureMapList& tex_maps)
{
    loosePrimitiveMaterials();
    for (unsigned i = 0; i < tex_maps.size() && i < m_materials.size(); i++)
    {
        if (m_materials[i]) m_materials[i]->changeTextureMap(tex_maps[i]);
    }
    bindPrimitiveMaterials();
}

void MeshPrimitive::createRenderElements()
{
    assert(m_geometry);
    assert(m_renderBuffer);
    unsigned elem_count = m_geometry->getSegmentCount();
    if (elem_count > m_materials.size()) elem_count = static_cast<unsigned>(m_materials.size());
    assert(elem_count > 0);
    m_elements.clear();
    m_elements.reserve(elem_count);
    for (unsigned i = 0; i < elem_count; i++)
    {
        m_elements.emplace_back(std::make_shared<Renderer::RenderElement>(m_renderBuffer, m_materials[i]->effectMaterial(), m_geometry->getSegment(i)));
    }
}

void MeshPrimitive::cleanupGeometry()
{
    m_elements.clear();
    m_geometry = nullptr;
    m_renderBuffer = nullptr;
}

void MeshPrimitive::bindPrimitiveMaterials()
{
    if (m_materials.empty()) return;
    for (auto& mat : m_materials)
    {
        if (mat == nullptr) continue;
        mat->assignShaderTextures();
    }
}

void MeshPrimitive::bindSegmentMaterial(unsigned index)
{
    if (index >= m_materials.size()) return;
    if (m_materials[index] == nullptr) return;
    m_materials[index]->assignShaderTextures();
}

void MeshPrimitive::loosePrimitiveMaterials()
{
    if (m_materials.empty()) return;
    for (auto& mat : m_materials)
    {
        if (mat == nullptr) continue;
        mat->unassignShaderTextures();
    }
}

void MeshPrimitive::looseSegmentMaterial(unsigned index)
{
    if (index >= m_materials.size()) return;
    if (m_materials[index] == nullptr) return;
    m_materials[index]->unassignShaderTextures();
}
