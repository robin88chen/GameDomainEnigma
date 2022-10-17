#include "RenderElementBuilder.h"
#include "RendererEvents.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectEvents.h"
#include <cassert>

#include "RenderElement.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

RenderElementBuilder::RenderElementBuilder(RendererManager* host, Engine::RenderBufferRepository* buffer_repository,
    Engine::EffectMaterialManager* material_manager)
{
    assert(host);
    assert(buffer_repository);
    assert(material_manager);
    m_hostManager = host;
    m_renderBufferRepository = buffer_repository;
    m_effectMaterialManager = material_manager;

    m_onRenderBufferBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnRenderBufferBuilt(e); });
    Frameworks::EventPublisher::Subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnBuildRenderBufferFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onEffectMaterialCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    Frameworks::EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    Frameworks::EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
}

RenderElementBuilder::~RenderElementBuilder()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;
}

void RenderElementBuilder::BuildRenderElement(const RenderElementPolicy& policy)
{
    m_policy = policy;
    m_buffer = nullptr;
    m_effect = nullptr;
    assert(m_renderBufferRepository);
    if (m_renderBufferRepository->HasRenderBuffer(m_policy.m_renderBufferSignature))
    {
        m_buffer = m_renderBufferRepository->QueryRenderBuffer(m_policy.m_renderBufferSignature);
    }
    assert(m_effectMaterialManager);
    if (m_effectMaterialManager->HasEffectMaterial(m_policy.m_effectMaterialName))
    {
        m_effect = m_effectMaterialManager->QueryEffectMaterial(m_policy.m_effectMaterialName);
    }
    if ((m_buffer) && (m_effect))
    {
        TryCompletingBuild();
    }
}

void RenderElementBuilder::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetSignature() != m_policy.m_renderBufferSignature) return;
    m_buffer = ev->GetBuffer();

    if ((m_buffer) && (m_effect))
    {
        TryCompletingBuild();
    }
}

void RenderElementBuilder::OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<BuildRenderElementFailed>(m_policy, ev->GetErrorCode()));
}

void RenderElementBuilder::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetFilename() != m_policy.m_effectMaterialName) return;
    m_effect = ev->GetEffect();
}

void RenderElementBuilder::OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed, Frameworks::IEvent>(e);
    if (!ev) return;
    Frameworks::EventPublisher::Post(std::make_shared<BuildRenderElementFailed>(m_policy, ev->GetErrorCode()));
}

void RenderElementBuilder::TryCompletingBuild()
{
    if ((!m_buffer) || (!m_effect)) return;
    auto element = std::make_shared<RenderElement>(m_buffer, m_effect, m_policy.m_segment);
    Frameworks::EventPublisher::Post(std::make_shared<RenderElementBuilder::RenderElementBuilt>(m_policy, element));
}
