/*********************************************************************
 * \file   Renderer.h
 * \brief  Renderer class, entity, shared_ptr, maintained by manager
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDERER_H
#define RENDERER_H

#include "RenderTargetClearingProperties.h"
#include <memory>
#include <string>
#include <system_error>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderTarget;

    class Renderer : public std::enable_shared_from_this<Renderer>
    {
    public:
        /** enum Render Group ID */
        enum class RenderListID
        {
            Sky = 0,
            Scene,
            Shadow,
            TerrainWater,
            DeferredLighting,
            OffSurface,  // particle, text primitive
            Overlay, // ui
            Count
        };
    public:
        Renderer(const std::string& name);
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        virtual ~Renderer();
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        /** get renderer's name */
        const std::string& GetName() { return m_name; };

        /** set render target */
        error SetRenderTarget(const std::shared_ptr<RenderTarget>& target);
        /** get render target */
        std::shared_ptr<RenderTarget> GetRenderTarget() const;

        virtual error ChangeClearingProperty(const RenderTargetClearChangingProperty& prop);
        /** clear render target */
        virtual error ClearRenderTarget();
        /** flip */
        error Flip();

    protected:
        std::string m_name;
        std::weak_ptr<RenderTarget> m_target;
    };
}

#endif // RENDERER_H
