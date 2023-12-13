/*********************************************************************
 * \file   RenderElement.h
 * \brief  Render Element, entity, shared_ptr, lifecycle controlled by Mesh
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDER_ELEMENT_H
#define RENDER_ELEMENT_H

#include "Geometries/GeometrySegment.h"
#include "GameEngine/EffectMaterial.h"
#include "MathLib/Matrix4.h"
#include <memory>
#include <system_error>

namespace Enigma::Engine
{
    class RenderBuffer;
    class EffectMaterial;
    class RenderLightingState;
};

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderElement : public std::enable_shared_from_this<RenderElement>
    {
    public:
        /** enum render element stationary state */
        enum class Mobility
        {
            Stationary = 0,
            Movable,
            Count,
        };
    public:
        RenderElement();
        RenderElement(const std::shared_ptr<Engine::RenderBuffer>& renderBuffer,
            const Engine::EffectMaterialPtr& effect, const Geometries::GeometrySegment& segment);
        RenderElement(const RenderElement&) = delete;
        RenderElement(RenderElement&&) = delete;
        ~RenderElement();
        RenderElement& operator=(const RenderElement&) = delete;
        RenderElement& operator=(RenderElement&&) = delete;

        std::shared_ptr<Engine::RenderBuffer> GetRenderBuffer() const;
        const Engine::EffectMaterialPtr& getEffectMaterial() const { return m_effectMaterial; };

        const Geometries::GeometrySegment& GetGeometrySegment() const { return m_segment; };

        error Draw(const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& state,
            const std::string& rendererTechnique);
        //future_err AsyncDraw(const Matrix4& mxWorld, const SpatialRenderStatePtr& state,
          //  const std::string& rendererTechnique);

        /** Draw by external effect material */
        error DrawExternal(const MathLib::Matrix4& mxWorld, const Engine::EffectMaterialPtr& effect);
        //future_err AsyncDrawExternal(const Matrix4& mxWorld, const EffectMaterialPtr& effect);

        inline void AddRendererStamp(unsigned int stamp) { m_rendererStamp |= stamp; };
        inline void RemoveRenderStamp(unsigned int stamp) { m_rendererStamp &= (~stamp); };
        inline unsigned int GetRendererStamp() const { return m_rendererStamp; };

        inline void AddActiveFrameFlag(unsigned int renderer_stamp_flag) { m_rendererActiveFrameFlag |= renderer_stamp_flag; };
        inline void RemoveActiveFrameFlag(unsigned int renderer_stamp_flag) { m_rendererActiveFrameFlag &= (~renderer_stamp_flag); };
        inline unsigned int GetActiveFrameFlag() const { return m_rendererActiveFrameFlag; };

    private:
        std::weak_ptr<Engine::RenderBuffer> m_renderBuffer;
        Engine::EffectMaterialPtr m_effectMaterial;
        Geometries::GeometrySegment m_segment;

        /// 這個 bit mask, 紀錄 element 放在哪個 renderer 中，可重複放，所以用 bit 紀錄
        unsigned int m_rendererStamp;
        /// element 是否有效的記號, 跟 renderer 相關, 所以也用 bit 存放
        unsigned int m_rendererActiveFrameFlag;
    };

    using RenderElementPtr = std::shared_ptr<RenderElement>;
}

#endif // RENDER_ELEMENT_H
