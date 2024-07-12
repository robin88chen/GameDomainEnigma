﻿#include "RenderBufferBuilder.h"
#include "RenderBufferEvents.h"
#include "RenderBufferRepository.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/PlatformLayer.h"
#include "EngineErrors.h"
#include "GraphicKernel/IGraphicAPI.h"
#include <cassert>

using namespace Enigma::Engine;

RenderBufferBuilder::RenderBufferBuilder(RenderBufferRepository* host)
{
    m_hostRepository = host;
    m_onVertexBufferCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onVertexBufferCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceVertexBufferCreated), m_onVertexBufferCreated);
    m_onVertexBufferUpdated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onVertexBufferUpdated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::VertexBufferResourceUpdated), m_onVertexBufferUpdated);

    m_onIndexBufferCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onIndexBufferCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceIndexBufferCreated), m_onIndexBufferCreated);
    m_onIndexBufferUpdated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onIndexBufferUpdated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::IndexBufferResourceUpdated), m_onIndexBufferUpdated);

    m_onVertexBufferUpdateFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onBufferUpdateFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::VertexBufferUpdateFailed), m_onVertexBufferUpdateFailed);
    m_onIndexBufferUpdateFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onBufferUpdateFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::IndexBufferUpdateFailed), m_onIndexBufferUpdateFailed);

    m_onVertexBufferBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onBufferBuilt(e); });
    Frameworks::EventPublisher::subscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onIndexBufferBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->onBufferBuilt(e); });
    Frameworks::EventPublisher::subscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);
}

RenderBufferBuilder::~RenderBufferBuilder()
{
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceVertexBufferCreated), m_onVertexBufferCreated);
    m_onVertexBufferCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::VertexBufferResourceUpdated), m_onVertexBufferUpdated);
    m_onVertexBufferUpdated = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceIndexBufferCreated), m_onIndexBufferCreated);
    m_onIndexBufferCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::IndexBufferResourceUpdated), m_onIndexBufferUpdated);
    m_onIndexBufferUpdated = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::VertexBufferUpdateFailed), m_onVertexBufferUpdateFailed);
    m_onVertexBufferUpdateFailed = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::IndexBufferUpdateFailed), m_onIndexBufferUpdateFailed);
    m_onIndexBufferUpdateFailed = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onVertexBufferBuilt = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);
    m_onIndexBufferBuilt = nullptr;
}

void RenderBufferBuilder::buildRenderBuffer(const RenderBufferPolicy& policy)
{
    assert(m_hostRepository);

    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    m_policy = policy;
    if (m_hostRepository->hasRenderBuffer(m_policy.m_signature))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<RenderBufferBuilt>(m_policy.m_renderBufferName, m_policy.m_signature,
            m_hostRepository->queryRenderBuffer(m_policy.m_signature)));
    }
    else
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateVertexBuffer>(
            m_policy.m_vtxBufferName, m_policy.m_sizeofVertex, m_policy.m_vtxBufferSize));
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateIndexBuffer>(
            m_policy.m_idxBufferName, m_policy.m_idxBufferSize));
    }
}

void RenderBufferBuilder::onVertexBufferCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceVertexBufferCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != m_policy.m_vtxBufferName) return;

    auto buffer = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IVertexBufferPtr>(ev->GetBufferName());
    if (!buffer)
    {
        Platforms::Debug::Printf("can't get vertex buffer asset %s", ev->GetBufferName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildRenderBufferFailed>(
            m_policy.m_renderBufferName, ErrorCode::findStashedAssetFail));
        return;
    }
    if (m_policy.m_vtxBuffer)
    {
        buffer.value()->update(m_policy.m_vtxBuffer.value());
    }
    else
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<VertexBufferBuilt>(m_policy.m_vtxBufferName));
    }
}

void RenderBufferBuilder::onVertexBufferUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::VertexBufferResourceUpdated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetVertexBufferName() != m_policy.m_vtxBufferName) return;

    Frameworks::EventPublisher::enqueue(std::make_shared<VertexBufferBuilt>(m_policy.m_vtxBufferName));
}

void RenderBufferBuilder::onIndexBufferCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceIndexBufferCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != m_policy.m_idxBufferName) return;

    auto buffer = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IIndexBufferPtr>(ev->GetBufferName());
    if (!buffer)
    {
        Platforms::Debug::Printf("can't get index buffer asset %s", ev->GetBufferName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildRenderBufferFailed>(
            m_policy.m_renderBufferName, ErrorCode::findStashedAssetFail));
        return;
    }
    if (m_policy.m_idxBuffer)
    {
        buffer.value()->update(m_policy.m_idxBuffer.value());
    }
    else
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<IndexBufferBuilt>(m_policy.m_idxBufferName));
    }
}

void RenderBufferBuilder::onIndexBufferUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::IndexBufferResourceUpdated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetIndexBufferName() != m_policy.m_idxBufferName) return;

    Frameworks::EventPublisher::enqueue(std::make_shared<IndexBufferBuilt>(m_policy.m_idxBufferName));
}

void RenderBufferBuilder::onBufferUpdateFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx_fail = std::dynamic_pointer_cast<Graphics::VertexBufferUpdateFailed, Frameworks::IEvent>(e);
    if (ev_vtx_fail)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildRenderBufferFailed>(
            m_policy.m_renderBufferName, ev_vtx_fail->GetError()));
        return;
    }
    auto ev_idx_fail = std::dynamic_pointer_cast<Graphics::IndexBufferUpdateFailed, Frameworks::IEvent>(e);
    if (ev_idx_fail)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildRenderBufferFailed>(
            m_policy.m_renderBufferName, ev_idx_fail->GetError()));
        return;
    }
}

void RenderBufferBuilder::onBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx = std::dynamic_pointer_cast<VertexBufferBuilt, Frameworks::IEvent>(e);
    if (ev_vtx)
    {
        if (ev_vtx->bufferName() != m_policy.m_vtxBufferName) return;
        m_vertexBuffer = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IVertexBufferPtr>(m_policy.m_vtxBufferName);
    }
    auto ev_idx = std::dynamic_pointer_cast<IndexBufferBuilt, Frameworks::IEvent>(e);
    if (ev_idx)
    {
        if (ev_idx->bufferName() != m_policy.m_idxBufferName) return;
        m_indexBuffer = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IIndexBufferPtr>(m_policy.m_idxBufferName);
    }
    if ((m_vertexBuffer) && (m_indexBuffer))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<RenderBufferBuilder::RenderBufferBuilt>(m_policy.m_renderBufferName, m_policy.m_signature,
            std::make_shared<RenderBuffer>(m_policy.m_signature, m_vertexBuffer, m_indexBuffer)));
    }
}
