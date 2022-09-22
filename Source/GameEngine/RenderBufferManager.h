/*********************************************************************
 * \file   RenderBufferManager.h
 * \brief  Render buffer repository
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_MANAGER_H
#define RENDER_BUFFER_MANAGER_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "RenderBufferSignature.h"
#include "RenderBufferBuildingPolicies.h"
#include "Frameworks/EventSubscriber.h"
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    using error = std::error_code;

    class RenderBuffer;
    class RenderBufferBuilder;

    class RenderBufferManager : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RenderBufferManager(Frameworks::ServiceManager* srv_manager);
        RenderBufferManager(const RenderBufferManager&) = delete;
        RenderBufferManager(RenderBufferManager&&) = delete;
        ~RenderBufferManager();
        RenderBufferManager& operator=(RenderBufferManager&) = delete;
        RenderBufferManager& operator=(RenderBufferManager&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        error BuildRenderBuffer(const RenderBufferPolicy& policy);

        bool HasRenderBuffer(const RenderBufferSignature& signature);
        std::shared_ptr<RenderBuffer> QueryRenderBuffer(const RenderBufferSignature& signature);

    private:
        void OnRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void OnRenderBufferBuildFailed(const Frameworks::IEventPtr& e);

    private:
        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onRenderBufferBuildFailed;

        using RenderBufferMap = std::unordered_map<RenderBufferSignature, std::weak_ptr<RenderBuffer>, RenderBufferSignature::_signature_hasher>;

        RenderBufferMap m_renderBuffers;
        std::recursive_mutex m_bufferMapLock;

        RenderBufferBuilder* m_builder;
        std::queue<RenderBufferPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;
    };
}

#endif // RENDER_BUFFER_MANAGER_H
