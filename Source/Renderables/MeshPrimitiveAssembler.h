/*********************************************************************
 * \file   MeshPrimitiveAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MESH_PRIMITIVE_ASSEMBLER_H
#define MESH_PRIMITIVE_ASSEMBLER_H

#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Primitives/PrimitiveAssembler.h"
#include "Geometries/GeometryId.h"
#include "Geometries/GeometryData.h"
#include "Renderer/Renderer.h"
#include <optional>

namespace Enigma::Renderables
{
    class MeshPrimitive;
    class PrimitiveMaterial;
    class PrimitiveMaterialAssembler;

    class MeshPrimitiveAssembler : public Primitives::PrimitiveAssembler
    {
    public:
        MeshPrimitiveAssembler(const Primitives::PrimitiveId& id);
        virtual ~MeshPrimitiveAssembler() override = default;
        MeshPrimitiveAssembler(const MeshPrimitiveAssembler&) = default;
        MeshPrimitiveAssembler(MeshPrimitiveAssembler&&) = default;
        MeshPrimitiveAssembler& operator=(const MeshPrimitiveAssembler&) = default;
        MeshPrimitiveAssembler& operator=(MeshPrimitiveAssembler&&) = default;

        void geometryId(const Geometries::GeometryId& id) { m_geometryId = id; }
        void geometry(const std::shared_ptr<Geometries::GeometryData>& geometry) { m_rawGeometry = geometry; }
        //void addEffect(const Engine::EffectMaterialId& id) { m_effects.push_back(id); }
        //void addTextureMap(const std::shared_ptr<Engine::EffectTextureMapAssembler>& texture_map) { m_textureMaps.emplace_back(texture_map); }
        void addMaterial(const std::shared_ptr<PrimitiveMaterial>& material);
        void addMaterial(const std::shared_ptr<PrimitiveMaterialAssembler>& material_assembler);
        void renderListID(Renderer::Renderer::RenderListID id) { m_renderListID = id; }
        void visualTechnique(const std::string& technique) { m_visualTechniqueSelection = technique; }
        void asNative(const std::string& file_at_path);

        virtual Engine::GenericDto assemble() const override;

    protected:
        Geometries::GeometryId m_geometryId;
        std::optional<std::shared_ptr<Geometries::GeometryData>> m_rawGeometry;
        std::vector<std::shared_ptr<PrimitiveMaterialAssembler>> m_materials;
        //std::vector<Engine::EffectMaterialId> m_effects;
        //std::vector<std::shared_ptr<Engine::EffectTextureMapAssembler>> m_textureMaps;
        Renderer::Renderer::RenderListID m_renderListID;
        std::string m_visualTechniqueSelection;
    };

    class MeshPrimitiveDisassembler : public Primitives::PrimitiveDisassembler
    {
    public:
        MeshPrimitiveDisassembler();
        virtual ~MeshPrimitiveDisassembler() override = default;
        MeshPrimitiveDisassembler(const MeshPrimitiveDisassembler&) = default;
        MeshPrimitiveDisassembler(MeshPrimitiveDisassembler&&) = default;
        MeshPrimitiveDisassembler& operator=(const MeshPrimitiveDisassembler&) = default;
        MeshPrimitiveDisassembler& operator=(MeshPrimitiveDisassembler&&) = default;

        [[nodiscard]] const Geometries::GeometryId& geometryId() const { return m_geometryId; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& rawGeometryDto() const { return m_rawGeometryDto; }
        [[nodiscard]] const std::vector<std::shared_ptr<PrimitiveMaterial>>& materials() const { return m_materials; }
        //[[nodiscard]] const std::vector<Engine::EffectMaterialId>& effects() const { return m_effects; }
        //[[nodiscard]] const std::vector<std::shared_ptr<Engine::EffectTextureMapDisassembler>>& textureMaps() const { return m_textureMaps; }
        [[nodiscard]] Renderer::Renderer::RenderListID renderListID() const { return m_renderListID; }
        [[nodiscard]] const std::string& visualTechniqueSelection() const { return m_visualTechniqueSelection; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        Geometries::GeometryId m_geometryId;
        std::optional<Engine::GenericDto> m_rawGeometryDto;
        //std::vector<Engine::EffectMaterialId> m_effects;
        //std::vector<std::shared_ptr<Engine::EffectTextureMapDisassembler>> m_textureMaps;
        std::vector<std::shared_ptr<PrimitiveMaterial>> m_materials;
        Renderer::Renderer::RenderListID m_renderListID;
        std::string m_visualTechniqueSelection;
    };

    /*class MeshPrimitiveMaterialMetaDisassembler
    {
    public:
        MeshPrimitiveMaterialMetaDisassembler() = default;
        MeshPrimitiveMaterialMetaDisassembler(const std::shared_ptr<MeshPrimitiveDisassembler>& mesh_disassembler);
        virtual ~MeshPrimitiveMaterialMetaDisassembler() = default;
        MeshPrimitiveMaterialMetaDisassembler(const MeshPrimitiveMaterialMetaDisassembler&) = default;
        MeshPrimitiveMaterialMetaDisassembler(MeshPrimitiveMaterialMetaDisassembler&&) = default;
        MeshPrimitiveMaterialMetaDisassembler& operator=(const MeshPrimitiveMaterialMetaDisassembler&) = default;
        MeshPrimitiveMaterialMetaDisassembler& operator=(MeshPrimitiveMaterialMetaDisassembler&&) = default;

        void replaceDuplicatedEffects(const std::shared_ptr<MeshPrimitive>& mesh_primitive);

        [[nodiscard]] const std::vector<Engine::EffectMaterialId>& effects() const { return m_effects; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMap>& textureMaps() const { return m_textureMaps; }

    protected:
        std::vector<Engine::EffectMaterialId> m_effects;
        std::vector<Engine::EffectTextureMap> m_textureMaps;
        //std::vector<std::shared_ptr<Engine::EffectTextureMapDisassembler>> m_textureMaps;
    };*/
}

#endif // MESH_PRIMITIVE_ASSEMBLER_H
