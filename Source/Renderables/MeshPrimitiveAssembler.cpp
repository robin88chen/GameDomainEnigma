#include "MeshPrimitiveAssembler.h"
#include "MeshPrimitive.h"
#include "Geometries/GeometryAssembler.h"
#include "PrimitiveMaterialAssembler.h"
#include <cassert>

using namespace Enigma::Renderables;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_GEOMETRY_ID = "GeometryId";
static std::string TOKEN_RAW_GEOMETRY = "RawGeometry";
static std::string TOKEN_MATERIALS = "Materials";
static std::string TOKEN_RENDER_LIST_ID = "RenderListId";
static std::string TOKEN_VISUAL_TECHNIQUE_SELECTION = "VisualTechniqueSelection";

MeshPrimitiveAssembler::MeshPrimitiveAssembler(const Primitives::PrimitiveId& id)
    : Primitives::PrimitiveAssembler(id), m_renderListID(Renderer::Renderer::RenderListID::Scene)
{
    m_factoryDesc = Engine::FactoryDesc(MeshPrimitive::TYPE_RTTI);
}

void MeshPrimitiveAssembler::addMaterial(const std::shared_ptr<PrimitiveMaterial>& material)
{
    assert(material);
    m_materials.emplace_back(PrimitiveMaterialAssembler::assembledAssemblerOf(material));
}

void MeshPrimitiveAssembler::addMaterial(const std::shared_ptr<PrimitiveMaterialAssembler>& material_assembler)
{
    assert(material_assembler);
    m_materials.emplace_back(material_assembler);
}

void MeshPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_factoryDesc.ClaimAsNative(file_at_path);
}

Enigma::Engine::GenericDto MeshPrimitiveAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addOrUpdate(TOKEN_GEOMETRY_ID, m_geometryId.name());
    if (m_rawGeometry)
    {
        std::shared_ptr<Geometries::GeometryAssembler> geometry_assembler = m_rawGeometry.value()->assembler();
        m_rawGeometry.value()->assemble(geometry_assembler);
        dto.addOrUpdate(TOKEN_RAW_GEOMETRY, geometry_assembler->assemble());
    }
    if (!m_materials.empty())
    {
        Engine::GenericDtoCollection materials;
        for (auto& mat_assembler : m_materials)
        {
            if (mat_assembler) materials.emplace_back(mat_assembler->assemble());
        }
        dto.addOrUpdate(TOKEN_MATERIALS, materials);
    }
    dto.addOrUpdate(TOKEN_RENDER_LIST_ID, static_cast<unsigned>(m_renderListID));
    if (!m_visualTechniqueSelection.empty())
    {
        dto.addOrUpdate(TOKEN_VISUAL_TECHNIQUE_SELECTION, m_visualTechniqueSelection);
    }
    return dto;
}

MeshPrimitiveDisassembler::MeshPrimitiveDisassembler() : Primitives::PrimitiveDisassembler(), m_renderListID(Renderer::Renderer::RenderListID::Scene)
{
    m_factoryDesc = Engine::FactoryDesc(MeshPrimitive::TYPE_RTTI);
}

void MeshPrimitiveDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) m_id = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_GEOMETRY_ID)) m_geometryId = v.value();
    if (const auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_RAW_GEOMETRY))
    {
        m_rawGeometryDto = v.value();
    }
    if (const auto ary = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_MATERIALS))
    {
        for (auto& mat_dto : ary.value())
        {
            auto material_disassembler = std::make_shared<PrimitiveMaterialDisassembler>(mat_dto);
            auto material = std::make_shared<PrimitiveMaterial>(material_disassembler->effectMaterialId(), material_disassembler->effectTextureMap());
            material->registerEventHandlersWhenNotReady();
            m_materials.emplace_back(material);
        }
    }
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_RENDER_LIST_ID)) m_renderListID = static_cast<Renderer::Renderer::RenderListID>(v.value());
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_VISUAL_TECHNIQUE_SELECTION)) m_visualTechniqueSelection = v.value();
}
