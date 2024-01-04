/*********************************************************************
 * \file   MeshPrimitive.h
 * \brief Mesh primitive, aggregate, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_H
#define _MESH_PRIMITIVE_H

#include "GameEngine/Primitive.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectTextureMap.h"
#include "GameEngine/GenericDto.h"
#include "Geometries/GeometryRepository.h"
#include "Frameworks/LazyStatus.h"
#include "Renderer.h"
#include <string>
#include <memory>
#include <system_error>
#include <vector>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderElement;
    class MeshPrimitiveDto;

    class MeshPrimitive : public Engine::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        using EffectMaterialList = std::vector<std::shared_ptr<Engine::EffectMaterial>>;
        using TextureMapList = std::vector<Engine::EffectTextureMap>;
    public:
        MeshPrimitive(const Engine::PrimitiveId& id);
        MeshPrimitive(const Engine::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository);
        MeshPrimitive(const MeshPrimitive&) = delete;  // non-copyable
        MeshPrimitive(MeshPrimitive&&) = delete;
        ~MeshPrimitive() override;
        MeshPrimitive& operator=(const MeshPrimitive&) = delete;
        MeshPrimitive& operator=(MeshPrimitive&&) = delete;

        virtual Engine::GenericDto serializeDto() const override;

        const std::string& getName() const { return m_name; }
        /** get geometry data */
        const Geometries::GeometryDataPtr& getGeometryData() const { return m_geometry; };

        /** get effect */
        std::shared_ptr<Engine::EffectMaterial> getEffectMaterial(unsigned index);
        /** get material count */
        unsigned getEffectMaterialCount() const;
        /** get texture map */
        const Engine::EffectTextureMap& getTextureMap(unsigned int index);
        /** get texture map size */
        unsigned getTextureMapCount() const;
        /** change specify semantic texture */
        void changeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        /** bind specify semantic texture, append new if semantic not existed */
        void bindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        /** bind specify semantic texture, append new if semantic not existed */
        void bindSemanticTextures(const Engine::EffectTextureMap::SegmentEffectTextures& texture_tuples);

        /** update render buffer */
        error updateRenderBuffer();
        /** update render buffer */
        error rangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count, std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count);

        /** render list id */
        Renderer::RenderListID renderListId() const { return m_renderListID; };
        Renderer::RenderListID& renderListId() { return m_renderListID; };

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
        virtual void changeEffectMaterialInSegment(unsigned index, const std::shared_ptr<Engine::EffectMaterial>& effect);
        /** change primitive's effect */
        virtual void changeEffectMaterials(const EffectMaterialList& effects);
        /** resize effect list */
        void resizeEffectMaterialVector(unsigned new_size) { m_effects.resize(new_size); };
        /** change segment's texture map */
        void changeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map);
        /** change primitive's texture map */
        void changeTextureMaps(const TextureMapList& tex_maps);
        /** resize texture map list */
        void resizeTextureMapVector(unsigned new_size) { m_textures.resize(new_size); };
        /** create render elements */
        void createRenderElements();
        //@}

        std::shared_ptr<Engine::Texture> findTextureBySemantic(const std::string& semantic) const;

    protected:
        MeshPrimitiveDto serializeMeshDto() const;

        void cleanupGeometry();

        /** bind primitive effect texture */
        void bindPrimitiveEffectTexture();
        /** bind segment effect texture */
        void bindSegmentEffectTexture(unsigned index);
        /** un-bind primitive effect texture */
        void loosePrimitiveEffectTexture();
        /** un-bind segment effect texture */
        void looseSegmentEffectTexture(unsigned index);

    protected:
        using RenderElementList = std::vector<std::shared_ptr<RenderElement>>;
        std::string m_name;
        Geometries::GeometryDataPtr m_geometry;
        Engine::RenderBufferPtr m_renderBuffer;
        RenderElementList m_elements;
        EffectMaterialList m_effects;
        TextureMapList m_textures;
        Renderer::RenderListID m_renderListID;  ///< default : render group scene
    };
}

#endif // _MESH_PRIMITIVE_H
