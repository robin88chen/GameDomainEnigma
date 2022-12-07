/*********************************************************************
 * \file   MeshPrimitive.h
 * \brief Mesh primitive, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_H
#define _MESH_PRIMITIVE_H

#include "GameEngine/Primitive.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectTextureMap.h"
#include "Renderer.h"
#include <string>
#include <memory>
#include <system_error>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderElement;

    class MeshPrimitive : public Engine::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        using EffectMaterialList = std::vector<Engine::EffectMaterialPtr>;
        using TextureMapList = std::vector<Engine::EffectTextureMap>;
    public:
        MeshPrimitive(const std::string& name);
        MeshPrimitive(const MeshPrimitive&);
        MeshPrimitive(MeshPrimitive&&);
        ~MeshPrimitive() override;
        MeshPrimitive& operator=(const MeshPrimitive&);
        MeshPrimitive& operator=(MeshPrimitive&&);

        const std::string& GetName() const { return m_name; }
        /** get geometry data */
        const Engine::GeometryDataPtr& GetGeometryData() const { return m_geometry; };

        /** get effect */
        Engine::EffectMaterialPtr GetEffectMaterial(unsigned index);
        /** get material count */
        unsigned GetEffectMaterialCount() const;
        /** get texture map */
        const Engine::EffectTextureMap& GetTextureMap(unsigned int index);
        /** get texture map size */
        unsigned GetTextureMapCount() const;

        /** update render buffer */
        error UpdateRenderBuffer();
        /** update render buffer */
        error RangedUpdateRenderBuffer(unsigned vtx_offset, unsigned vtx_count, std::optional<unsigned> idx_offset, std::optional<unsigned> idx_count);

        /** render list id */
        Renderer::RenderListID RenderListID() const { return m_renderListID; };
        Renderer::RenderListID& RenderListID() { return m_renderListID; };

        /** insert to renderer */
        virtual error InsertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState) override;
        /** remove from renderer */
        virtual error RemoveFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer) override;

        /** calculate bounding volume */
        virtual void CalculateBoundingVolume(bool axis_align) override;

        /** update world transform */
        virtual void UpdateWorldTransform(const MathLib::Matrix4& mxWorld) override;

        /** @name building mesh primitive */
        //@{
        /** link geometry object and render buffer */
        void LinkGeometryData(const Engine::GeometryDataPtr& geo, const Engine::RenderBufferPtr& render_buffer);
        /** change segment's effect */
        virtual void ChangeEffectMaterialInSegment(unsigned index, const Engine::EffectMaterialPtr& effect);
        /** change primitive's effect */
        virtual void ChangeEffectMaterial(const EffectMaterialList& effects);
        /** resize effect list */
        void ResizeEffectMaterialVector(unsigned new_size) { m_effects.resize(new_size); };
        /** change segment's texture map */
        void ChangeTextureMapInSegment(unsigned index, const Engine::EffectTextureMap& tex_map);
        /** change primitive's texture map */
        void ChangeTextureMap(const TextureMapList& tex_maps);
        /** resize texture map list */
        void ResizeTextureMapVector(unsigned new_size) { m_textures.resize(new_size); };
        /** create render elements */
        void CreateRenderElements();
        //@}

    protected:
        void CleanupGeometry();

        /** bind primitive effect texture */
        void BindPrimitiveEffectTexture();
        /** bind segment effect texture */
        void BindSegmentEffectTexture(unsigned index);
        /** un-bind primitive effect texture */
        void LoosePrimitiveEffectTexture();
        /** un-bind segment effect texture */
        void LooseSegmentEffectTexture(unsigned index);

    protected:
        using RenderElementList = std::vector<std::shared_ptr<RenderElement>>;
        std::string m_name;
        Engine::GeometryDataPtr m_geometry;
        Engine::RenderBufferPtr m_renderBuffer;
        RenderElementList m_elements;
        EffectMaterialList m_effects;
        TextureMapList m_textures;
        Renderer::RenderListID m_renderListID;  ///< default : render group scene
    };
    using MeshPrimitivePtr = std::shared_ptr<MeshPrimitive>;
}

#endif // _MESH_PRIMITIVE_H
