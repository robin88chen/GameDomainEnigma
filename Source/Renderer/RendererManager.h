/********************************************************************
 * \file   RendererManager.h
 * \brief  Renderer Manager, manage renderer(s) and render targets
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include "RenderTarget.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/Rtti.h"
#include "GameEngine/IRenderer.h"
#include <system_error>
#include <memory>
#include <unordered_map>
#include <functional>

namespace Enigma::Renderer
{
    using error = std::error_code;

    using  CustomRendererFactoryFunc = std::function<Engine::IRendererPtr(const std::string&)>;

    /** Renderer Manager Service */
    class RendererManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RendererManager(Frameworks::ServiceManager* srv_mngr);
        RendererManager(const RendererManager&) = delete;
        RendererManager(RendererManager&&) = delete;
        virtual ~RendererManager();
        RendererManager& operator=(const RendererManager&) = delete;
        RendererManager& operator=(RendererManager&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        /** register renderer factory */
        static void RegisterCustomRendererFactory(const std::string& type_name, CustomRendererFactoryFunc fn);

        /** create renderer */
        error CreateRenderer(const std::string& name);
        /** create custom renderer */
        error CreateCustomRenderer(const std::string& type_name, const std::string& name);

        /** destroy renderer by name : remove from map, & destroy  */
        error DestroyRenderer(const std::string& name);
        /** get renderer */
        Engine::IRendererPtr GetRenderer(const std::string& name) const;

        /** create render target */
        error CreateRenderTarget(const std::string& name, RenderTarget::PrimaryType primary);

        /** destroy render target by name : remove from map, & destroy  */
        error DestroyRenderTarget(const std::string& name);
        /** get render target */
        RenderTargetPtr GetRenderTarget(const std::string& name) const;
        /** get primary render target */
        RenderTargetPtr GetPrimaryRenderTarget() const;

        /** @name command handler */
        //@{
        void DoResizingPrimaryTarget(const Frameworks::ICommandPtr& c);
        //@}

    protected:
        void ClearAllRenderer();
        void ClearAllRenderTarget();

    protected:
        using RendererMap = std::unordered_map<std::string, Engine::IRendererPtr>;
        using RenderTargetMap = std::unordered_map<std::string, RenderTargetPtr>;
        RendererMap m_renderers;
        RenderTargetMap m_renderTargets;

        std::string m_primaryRenderTargetName;

        unsigned int m_accumulateRendererStamp;  ///< 記錄哪些stamp bit已經被使用過

        using CustomRendererFactoryTable = std::unordered_map<std::string, CustomRendererFactoryFunc>;
        static CustomRendererFactoryTable m_customRendererFactoryTable;

        Frameworks::CommandSubscriberPtr m_doResizingPrimaryTarget;
    };
};


#endif // RENDERER_MANAGER_H
