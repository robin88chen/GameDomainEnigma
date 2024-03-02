/*********************************************************************
 * \file   RenderPack.h
 * \brief  Render Pack, value object
 * 主要是將 Render Element 打包，不做共享的實體物件。
 * 要解決相同的 Render Element 被放到不同的 Renderer (sync or async) 時，有不同的狀態。
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDER_PACK_H
#define RENDER_PACK_H

#include "MathLib/Matrix4.h"
#include "GameEngine/RenderLightingState.h"
#include <memory>

namespace Enigma::Renderer
{
    class RenderElement;

    class RenderPack
    {
    public:
        RenderPack();
        RenderPack(const std::shared_ptr<RenderElement>& element,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lighting_state);
        RenderPack(const RenderPack& pack);
        RenderPack(RenderPack&& pack) noexcept;
        ~RenderPack();

        RenderPack& operator=(const RenderPack& pack);
        RenderPack& operator=(RenderPack&& pack) noexcept;
        bool operator==(const RenderPack& p) const;
        bool operator!=(const RenderPack& p) const;

        const std::shared_ptr<RenderElement>& getRenderElement();
        const std::shared_ptr<RenderElement>& getRenderElement() const;
        MathLib::Matrix4 getWorldTransform();
        const MathLib::Matrix4& getWorldTransform() const;
        void setWorldTransform(const MathLib::Matrix4& mxWorld);
        const Engine::RenderLightingState& getRenderLightingState() const;
        void setRenderLightingState(const Engine::RenderLightingState& state);

        float getSquareCameraDistance() { return m_squareCameraDistance; }
        float getSquareCameraDistance() const { return m_squareCameraDistance; }
        void calcSquareCameraDistance(const MathLib::Vector3& camera_loc);

    protected:
        std::shared_ptr<RenderElement> m_element;
        MathLib::Matrix4 m_worldTransform;
        float m_squareCameraDistance;
        Engine::RenderLightingState m_renderLightingState;
    };
}

#endif // RENDER_PACK_H
