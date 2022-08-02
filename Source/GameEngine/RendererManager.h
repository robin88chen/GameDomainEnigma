/********************************************************************
 * \file   RendererManager.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_MANAGER_H
#define RENDERER_MANAGER_H

#include "RenderTarget.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/Rtti.h"
#include <system_error>
#include <memory>
#include <unordered_map>
#include <functional>

namespace Enigma::Engine
{
    using error = std::error_code;

    class Renderer;
    using RendererPtr = std::shared_ptr<Renderer>;
    using  CustomRendererFactoryFunc = std::function<RendererPtr(const std::string&)>;

    /** Renderer Manager Service */
    class RendererManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RendererManager(Frameworks::ServiceManager* srv_mngr);
        RendererManager(const RendererManager&) = delete;
        virtual ~RendererManager();
        RendererManager& operator=(const RendererManager&) = delete;

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
        RendererPtr GetRenderer(const std::string& name) const;

        /** select renderer technique */
        void SelectRendererTechnique(const std::string& renderer_name, const std::string& technique_name);

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
        void HandleResizingPrimaryTarget(const Frameworks::ICommandPtr& c);
        //@}

    protected:
        void ClearAllRenderer();
        void ClearAllRenderTarget();

    protected:
        using RendererMap = std::unordered_map<std::string, RendererPtr>;
        using RenderTargetMap = std::unordered_map<std::string, RenderTargetPtr>;
        RendererMap m_mapRenderer;
        RenderTargetMap m_mapRenderTarget;

        std::string m_primaryRenderTargetName;

        unsigned int m_accumulateRendererStamp;  ///< 記錄哪些stamp bit已經被使用過

        using CustomRendererFactoryTable = std::unordered_map<std::string, CustomRendererFactoryFunc>;
        static CustomRendererFactoryTable m_customRendererFactoryTable;

        Frameworks::CommandSubscriberPtr m_handleResizingPrimaryTarget;
    };
};


#endif // RENDERER_MANAGER_H
