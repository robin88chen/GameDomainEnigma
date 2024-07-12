/*********************************************************************
 * \file   RenderBufferRepository.h
 * \brief  Render buffer repository
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_REPOSITORY_H
#define RENDER_BUFFER_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/Command.h"
#include "RenderBufferSignature.h"
#include "RenderBufferBuildingPolicies.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    using error = std::error_code;

    class RenderBuffer;
    class RenderBufferBuilder;

    class RenderBufferRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RenderBufferRepository(Frameworks::ServiceManager* srv_manager);
        RenderBufferRepository(const RenderBufferRepository&) = delete;
        RenderBufferRepository(RenderBufferRepository&&) = delete;
        ~RenderBufferRepository() override;
        RenderBufferRepository& operator=(RenderBufferRepository&) = delete;
        RenderBufferRepository& operator=(RenderBufferRepository&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        error buildRenderBuffer(const RenderBufferPolicy& policy);

        bool hasRenderBuffer(const RenderBufferSignature& signature);
        std::shared_ptr<RenderBuffer> queryRenderBuffer(const RenderBufferSignature& signature);

    private:
        void onRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void onBuildRenderBufferFailed(const Frameworks::IEventPtr& e);
        void buildRenderBuffer(const Frameworks::ICommandPtr& c);

    private:
        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;
        Frameworks::CommandSubscriberPtr m_buildRenderBuffer;

        using RenderBufferMap = std::unordered_map<RenderBufferSignature, std::weak_ptr<RenderBuffer>, RenderBufferSignature::_signature_hasher>;

        RenderBufferMap m_renderBuffers;
        std::recursive_mutex m_bufferMapLock;

        RenderBufferBuilder* m_builder;
        std::queue<RenderBufferPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;
    };
}

#endif // RENDER_BUFFER_REPOSITORY_H
