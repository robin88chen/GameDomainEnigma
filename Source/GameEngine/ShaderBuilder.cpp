#include "ShaderBuilder.h"
#include "ShaderEvents.h"
#include "ShaderRepository.h"
#include "EngineErrors.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicCommands.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

ShaderBuilder::ShaderBuilder(ShaderRepository* host)
{
    m_hostRepository = host;

    m_vtxShader = nullptr;
    m_pixShader = nullptr;
    m_layout = nullptr;
    m_program = nullptr;

    m_onVertexShaderCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexShaderCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexShaderCompiled(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexLayoutCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexLayoutCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);

    m_onPixelShaderCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnPixelShaderCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnPixelShaderCompiled(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::PixelShaderCompiled), m_onPixelShaderCompiled);

    m_onVertexShaderCompileFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderCompileFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::VertexShaderCompileFailed), m_onVertexShaderCompileFailed);
    m_onPixelShaderCompileFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderCompileFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::PixelShaderCompileFailed), m_onPixelShaderCompileFailed);

    m_onVertexShaderBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    Frameworks::EventPublisher::subscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onPixelShaderBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    Frameworks::EventPublisher::subscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
    m_onShaderProgramCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderProgramCreated(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::DeviceShaderProgramCreated), m_onShaderProgramCreated);

    m_onShaderProgramLinkFailed = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderProgramLinkFailed(e); });
    Frameworks::EventPublisher::subscribe(typeid(Graphics::ShaderProgramLinkFailed), m_onShaderProgramLinkFailed);
}

ShaderBuilder::~ShaderBuilder()
{
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexShaderCompiled = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);
    m_onVertexLayoutCreated = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCreated = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::PixelShaderCompiled), m_onPixelShaderCompiled);
    m_onPixelShaderCompiled = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::VertexShaderCompileFailed), m_onVertexShaderCompileFailed);
    m_onVertexShaderCompileFailed = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::PixelShaderCompileFailed), m_onPixelShaderCompileFailed);
    m_onPixelShaderCompileFailed = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onVertexShaderBuilt = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
    m_onPixelShaderBuilt = nullptr;
    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::DeviceShaderProgramCreated), m_onShaderProgramCreated);
    m_onShaderProgramCreated = nullptr;

    Frameworks::EventPublisher::unsubscribe(typeid(Graphics::ShaderProgramLinkFailed), m_onShaderProgramLinkFailed);
    m_onShaderProgramLinkFailed = nullptr;

    m_vtxShader = nullptr;
    m_pixShader = nullptr;
    m_layout = nullptr;
    m_program = nullptr;
}

void ShaderBuilder::BuildShaderProgram(const ShaderProgramPolicy& policy)
{
    assert(m_hostRepository);

    m_vtxShader = nullptr;
    m_pixShader = nullptr;
    m_layout = nullptr;
    m_program = nullptr;

    m_policy = policy;
    if (m_hostRepository->HasVertexShader(m_policy.m_vtxShaderName))
    {
        //m_vtxShader = m_hostManager->QueryVertexShader(m_policy.m_vtxShaderName);
        Frameworks::EventPublisher::enqueue(std::make_shared<VertexShaderBuilt>(m_policy.m_vtxShaderName));
    }
    else
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateVertexShader>(m_policy.m_vtxShaderName));
    }
    if (m_hostRepository->HasPixelShader(m_policy.m_pixShaderName))
    {
        //m_pixShader = m_hostManager->QueryPixelShader(m_policy.m_pixShaderName);
        Frameworks::EventPublisher::enqueue(std::make_shared<PixelShaderBuilt>(m_policy.m_pixShaderName));
    }
    else
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreatePixelShader>(m_policy.m_pixShaderName));
    }
}

void ShaderBuilder::OnVertexShaderCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceVertexShaderCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_policy.m_vtxShaderName) return;

    auto shader = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IVertexShaderPtr>(ev->getName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get vertex shader asset %s\n", ev->getName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    shader.value()->Compile(m_policy.m_vtxShaderCode.m_code, m_policy.m_vtxShaderCode.m_profile, m_policy.m_vtxShaderCode.m_entry);
}

void ShaderBuilder::OnVertexShaderCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::VertexShaderCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_policy.m_vtxShaderName) return;
    auto shader = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IVertexShaderPtr>(ev->GetShaderName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get vertex shader asset %s\n", ev->GetShaderName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    if (m_hostRepository->HasVertexLayout(m_policy.m_vtxLayoutName))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<VertexShaderBuilt>(shader.value()->getName()));
    }
    else
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateVertexDeclaration>(
            m_policy.m_vtxLayoutName, m_policy.m_vtxFormatCode, shader.value()));
    }
}

void ShaderBuilder::OnVertexLayoutCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceVertexDeclarationCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_policy.m_vtxLayoutName) return;

    auto layout = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IVertexDeclarationPtr>(ev->getName());
    if (!layout)
    {
        Platforms::Debug::Printf("can't get vertex layout asset %s\n", ev->getName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    Frameworks::EventPublisher::enqueue(std::make_shared<VertexShaderBuilt>(m_policy.m_vtxShaderName));
}

void ShaderBuilder::OnPixelShaderCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DevicePixelShaderCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_policy.m_pixShaderName) return;

    auto shader = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IPixelShaderPtr>(ev->getName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get pixel shader asset %s\n", ev->getName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    shader.value()->Compile(m_policy.m_pixShaderCode.m_code, m_policy.m_pixShaderCode.m_profile,
        m_policy.m_pixShaderCode.m_entry);
}

void ShaderBuilder::OnPixelShaderCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::PixelShaderCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_policy.m_pixShaderName) return;
    auto shader = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IPixelShaderPtr>(ev->GetShaderName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get pixel shader asset %s\n", ev->GetShaderName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    Frameworks::EventPublisher::enqueue(std::make_shared<PixelShaderBuilt>(m_policy.m_pixShaderName));
}

void ShaderBuilder::OnShaderCompileFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx_fail = std::dynamic_pointer_cast<Graphics::VertexShaderCompileFailed, Frameworks::IEvent>(e);
    if (ev_vtx_fail)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, Graphics::ErrorCode::compileShader));
        return;
    }
    auto ev_pix_fail = std::dynamic_pointer_cast<Graphics::PixelShaderCompileFailed, Frameworks::IEvent>(e);
    if (ev_pix_fail)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, Graphics::ErrorCode::compileShader));
        return;
    }
}

void ShaderBuilder::OnShaderBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx = std::dynamic_pointer_cast<VertexShaderBuilt, Frameworks::IEvent>(e);
    if (ev_vtx)
    {
        if (ev_vtx->GetShaderName() != m_policy.m_vtxShaderName) return;
        if (m_hostRepository->HasVertexShader(m_policy.m_vtxShaderName))
        {
            m_vtxShader = m_hostRepository->QueryVertexShader(m_policy.m_vtxShaderName);
        }
        else
        {
            m_vtxShader = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IVertexShaderPtr>(m_policy.m_vtxShaderName);
        }
        if (m_hostRepository->HasVertexLayout(m_policy.m_vtxLayoutName))
        {
            m_layout = m_hostRepository->QueryVertexLayout(m_policy.m_vtxLayoutName);
        }
        else
        {
            m_layout = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IVertexDeclarationPtr>(m_policy.m_vtxLayoutName);
        }
    }
    auto ev_pix = std::dynamic_pointer_cast<PixelShaderBuilt, Frameworks::IEvent>(e);
    if (ev_pix)
    {
        if (ev_pix->GetShaderName() != m_policy.m_pixShaderName) return;
        if (m_hostRepository->HasPixelShader(m_policy.m_pixShaderName))
        {
            m_pixShader = m_hostRepository->QueryPixelShader(m_policy.m_pixShaderName);
        }
        else
        {
            m_pixShader = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IPixelShaderPtr>(m_policy.m_pixShaderName);
        }
    }
    if ((m_vtxShader) && (m_pixShader))
    {
        Frameworks::CommandBus::enqueue(std::make_shared<Graphics::CreateShaderProgram>(
            m_policy.m_programName, m_vtxShader, m_pixShader, m_layout));
    }
}

void ShaderBuilder::OnShaderProgramCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceShaderProgramCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_policy.m_programName) return;
    auto program = Graphics::IGraphicAPI::instance()->TryFindGraphicAsset<Graphics::IShaderProgramPtr>(ev->getName());
    if (!program)
    {
        Platforms::Debug::Printf("can't get shader program asset %s\n", ev->getName().c_str());
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, ErrorCode::findStashedAssetFail));
        return;
    }
    m_program = Graphics::IGraphicAPI::instance()->GetGraphicAsset<Graphics::IShaderProgramPtr>(m_policy.m_programName);
    Frameworks::EventPublisher::enqueue(std::make_shared<ShaderProgramBuilt>(m_policy.m_programName));
}

void ShaderBuilder::OnShaderProgramLinkFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_link_fail = std::dynamic_pointer_cast<Graphics::ShaderProgramLinkFailed, Frameworks::IEvent>(e);
    if (ev_link_fail)
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<BuildShaderProgramFailed>(
            m_policy.m_programName, Graphics::ErrorCode::linkShaderProgram));
        return;
    }
}

