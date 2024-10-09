/*********************************************************************
 * \file   MeshPrimitive.h
 * \brief Mesh primitive, aggregate, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_H
#define _MESH_PRIMITIVE_H

#include "Primitives/Primitive.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectTextureMap.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderElement.h"
#include "PrimitiveMaterial.h"
#include "GameEngine/EffectSemanticTexture.h"
#include <string>
#include <memory>
#include <system_error>
#include <vector>

namespace Enigma::Renderables
{
    using error = std::error_code;

    class PrimitiveMaterial;

    class MeshPrimitive : public Primitives::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        using EffectMaterialList = std::vector<std::shared_ptr<Engine::EffectMaterial>>;
        using TextureMapList = std::vector<Engine::EffectTextureMap>;
    public:
        MeshPrimitive(const Primitives::PrimitiveId& id);
        MeshPrimitive(const MeshPrimitive&) = delete;  // non-copyable
        MeshPrimitive(MeshPrimitive&&) = delete;
        ~MeshPrimitive() override;
        MeshPrimitive& operator=(const MeshPrimitive&) = delete;
        MeshPrimitive& operator=(MeshPrimitive&&) = delete;

        static std::shared_ptr<MeshPrimitive> create(const Primitives::PrimitiveId& id);

        virtual std::shared_ptr<Primitives::PrimitiveAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<Primitives::PrimitiveAssembler>& assembler) const override;
        virtual std::shared_ptr<Primitives::PrimitiveDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<Primitives::PrimitiveDisassembler>& disassembler) override; ///< that's double dispatch

        /** get geometry data */
        const Geometries::GeometryDataPtr& getGeometryData() const { return m_geometry; };

        std::shared_ptr<PrimitiveMaterial> getMaterial(unsigned index);
        const std::vector<std::shared_ptr<PrimitiveMaterial>>& materials() const { return m_materials; }
        unsigned getMaterialCount() const;
        virtual void changeMaterials(const std::vector<std::shared_ptr<PrimitiveMaterial>>& materials);
        virtual void rebindMaterials();

        void changeSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture);
        void bindSemanticTexture(const Engine::EffectSemanticTexture& semantic_texture);
        void bindSemanticTextures(const std::vector<Engine::EffectSemanticTexture>& textures);

        /** update render buffer */
        error updateRenderBuffer();
        /** update render buffer */
        error rangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count, std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count);

        /** render list id */
        Renderer::Renderer::RenderListID renderListId() const { return m_renderListID; };
        void renderListId(Renderer::Renderer::RenderListID id) { m_renderListID = id; };

        /** insert to renderer */
        virtual error insertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState) override;
        /** remove from renderer */
        virtual error removeFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer) override;

        /** calculate bounding volume */
        virtual void calculateBoundingVolume(bool axis_align) override;

        /** update world transform */
        virtual void updateWorldTransform(const MathLib::Matrix4& mxWorld) override;

        virtual void selectVisualTechnique(const std::string& techniqueName) override;

        /** @name building mesh primitive */
        //@{
        /** link geometry object and render buffer */
        void linkGeometryData(const Geometries::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer);
        /** change segment's effect */
        virtual void changeEffectInSegment(unsigned index, const std::shared_ptr<Engine::EffectMaterial>& effect);
        /** change primitive's effect */
        virtual void changeEffects(const EffectMaterialList& effects);
        /** change segment's texture map */
        void changeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map);
        /** change primitive's texture map */
        void changeTextureMaps(const TextureMapList& tex_maps);
        /** create render elements */
        void createRenderElements();
        //@}

    protected:
        void cleanupGeometry();

        void bindPrimitiveMaterials();
        void bindSegmentMaterial(unsigned index);
        void loosePrimitiveMaterials();
        void looseSegmentMaterial(unsigned index);

    protected:
        using RenderElementList = std::vector<std::shared_ptr<Renderer::RenderElement>>;
        Geometries::GeometryDataPtr m_geometry;
        Engine::RenderBufferPtr m_renderBuffer;
        RenderElementList m_elements;
        std::vector<std::shared_ptr<PrimitiveMaterial>> m_materials;
        Renderer::Renderer::RenderListID m_renderListID;  ///< default : render group scene
    };
}

#endif // _MESH_PRIMITIVE_H
