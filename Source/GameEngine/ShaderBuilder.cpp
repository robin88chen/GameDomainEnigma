#include "ShaderBuilder.h"
#include "ShaderEvents.h"
#include "ShaderManager.h"
#include "EngineErrors.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Engine;

ShaderBuilder::ShaderBuilder(ShaderManager* host)
{
    m_hostManager = host;

    m_vtxShader = nullptr;
    m_pixShader = nullptr;

    m_onVertexShaderCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexShaderCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexShaderCompiled(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexLayoutCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnVertexLayoutCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);

    m_onPixelShaderCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnPixelShaderCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCompiled = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnPixelShaderCompiled(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::PixelShaderCompiled), m_onPixelShaderCompiled);

    m_onVertexShaderBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    Frameworks::EventPublisher::Subscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onPixelShaderBuilt = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    Frameworks::EventPublisher::Subscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
    m_onShaderProgramCreated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { this->OnShaderProgramCreated(e); });
    Frameworks::EventPublisher::Subscribe(typeid(Graphics::DeviceShaderProgramCreated), m_onShaderProgramCreated);
}

ShaderBuilder::~ShaderBuilder()
{
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexShaderCompiled = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);
    m_onVertexLayoutCreated = nullptr;

    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCreated = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::PixelShaderCompiled), m_onPixelShaderCompiled);
    m_onPixelShaderCompiled = nullptr;

    Frameworks::EventPublisher::Unsubscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onVertexShaderBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
    m_onPixelShaderBuilt = nullptr;
    Frameworks::EventPublisher::Unsubscribe(typeid(Graphics::DeviceShaderProgramCreated), m_onShaderProgramCreated);
    m_onShaderProgramCreated = nullptr;

    m_vtxShader = nullptr;
    m_pixShader = nullptr;
}

void ShaderBuilder::BuildShaderProgram(const ShaderProgramPolicy& policy)
{
    assert(m_hostManager);

    m_policy = policy;
    if (m_hostManager->HasVertexShader(m_policy.m_vtxShaderName))
    {
        m_vtxShader = m_hostManager->QueryVertexShader(m_policy.m_vtxShaderName);
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew VertexShaderBuilt{ m_policy.m_vtxShaderName } });
    }
    else
    {
        BuildVertexShader();
    }
    if (m_hostManager->HasPixelShader(m_policy.m_pixShaderName))
    {
        m_pixShader = m_hostManager->QueryPixelShader(m_policy.m_pixShaderName);
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew PixelShaderBuilt{ m_policy.m_pixShaderName } });
    }
    else
    {
        BuildPixelShader();
    }
}

void ShaderBuilder::BuildVertexShader() const
{
    if (Graphics::IGraphicAPI::Instance()->UseAsync())
    {
        Graphics::IGraphicAPI::Instance()->AsyncCreateVertexShader(m_policy.m_vtxShaderName);
    }
    else
    {
        Graphics::IGraphicAPI::Instance()->CreateVertexShader(m_policy.m_vtxShaderName);
    }
}

void ShaderBuilder::BuildPixelShader() const
{
    if (Graphics::IGraphicAPI::Instance()->UseAsync())
    {
        Graphics::IGraphicAPI::Instance()->AsyncCreatePixelShader(m_policy.m_pixShaderName);
    }
    else
    {
        Graphics::IGraphicAPI::Instance()->CreatePixelShader(m_policy.m_pixShaderName);
    }
}

void ShaderBuilder::OnVertexShaderCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceVertexShaderCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.m_vtxShaderName) return;

    auto shader = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IVertexShaderPtr>(ev->GetName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get vertex shader asset %s", ev->GetName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    if (Graphics::IGraphicAPI::Instance()->UseAsync())
    {
        shader.value()->AsyncCompileCode(m_policy.m_vtxShaderCode.m_code, m_policy.m_vtxShaderCode.m_profile,
            m_policy.m_vtxShaderCode.m_entry);
    }
    else
    {
        shader.value()->CompileCode(m_policy.m_vtxShaderCode.m_code, m_policy.m_vtxShaderCode.m_profile,
            m_policy.m_vtxShaderCode.m_entry);
    }
}

void ShaderBuilder::OnVertexShaderCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::VertexShaderCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_policy.m_vtxShaderName) return;
    auto shader = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IVertexShaderPtr>(ev->GetShaderName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get vertex shader asset %s", ev->GetShaderName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    std::string existed_layout = Graphics::IGraphicAPI::Instance()->QueryVertexDeclarationName(m_policy.m_vtxFormatCode, shader.value());
    if (!existed_layout.empty())
    {
        m_policy.m_vtxLayoutName = existed_layout;
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew VertexShaderBuilt{ shader.value()->GetName() } });
    }
    else
    {
        if (Graphics::IGraphicAPI::Instance()->UseAsync())
        {
            Graphics::IGraphicAPI::Instance()->AsyncCreateVertexDeclaration(
                m_policy.m_vtxLayoutName, m_policy.m_vtxFormatCode, shader.value());
        }
        else
        {
            Graphics::IGraphicAPI::Instance()->CreateVertexDeclaration(
                m_policy.m_vtxLayoutName, m_policy.m_vtxFormatCode, shader.value());
        }
    }
}

void ShaderBuilder::OnVertexLayoutCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceVertexDeclarationCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.m_vtxLayoutName) return;

    auto layout = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IVertexDeclarationPtr>(ev->GetName());
    if (!layout)
    {
        Platforms::Debug::Printf("can't get vertex layout asset %s", ev->GetName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew VertexShaderBuilt{ m_policy.m_vtxShaderName } });
}

void ShaderBuilder::OnPixelShaderCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DevicePixelShaderCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.m_pixShaderName) return;

    auto shader = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IPixelShaderPtr>(ev->GetName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get pixel shader asset %s", ev->GetName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    if (Graphics::IGraphicAPI::Instance()->UseAsync())
    {
        shader.value()->AsyncCompileCode(m_policy.m_pixShaderCode.m_code, m_policy.m_pixShaderCode.m_profile,
            m_policy.m_pixShaderCode.m_entry);
    }
    else
    {
        shader.value()->CompileCode(m_policy.m_pixShaderCode.m_code, m_policy.m_pixShaderCode.m_profile,
            m_policy.m_pixShaderCode.m_entry);
    }
}

void ShaderBuilder::OnPixelShaderCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::PixelShaderCompiled, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_policy.m_pixShaderName) return;
    auto shader = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IPixelShaderPtr>(ev->GetShaderName());
    if (!shader)
    {
        Platforms::Debug::Printf("can't get pixel shader asset %s", ev->GetShaderName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew PixelShaderBuilt{ m_policy.m_pixShaderName } });
}

void ShaderBuilder::OnShaderBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx = std::dynamic_pointer_cast<VertexShaderBuilt, Frameworks::IEvent>(e);
    if (ev_vtx)
    {
        if (ev_vtx->GetShaderName() != m_policy.m_vtxShaderName) return;
        m_vtxShader = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::IVertexShaderPtr>(m_policy.m_vtxShaderName);
    }
    auto ev_pix = std::dynamic_pointer_cast<PixelShaderBuilt, Frameworks::IEvent>(e);
    if (ev_pix)
    {
        if (ev_pix->GetShaderName() != m_policy.m_pixShaderName) return;
        m_pixShader = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::IPixelShaderPtr>(m_policy.m_pixShaderName);
    }
    if ((m_vtxShader) && (m_pixShader))
    {
        if (Graphics::IGraphicAPI::Instance()->UseAsync())
        {
            Graphics::IGraphicAPI::Instance()->AsyncCreateShaderProgram(m_policy.m_programName, m_vtxShader, m_pixShader);
        }
        else
        {
            Graphics::IGraphicAPI::Instance()->CreateShaderProgram(m_policy.m_programName, m_vtxShader, m_pixShader);
        }
    }
}

void ShaderBuilder::OnShaderProgramCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Graphics::DeviceShaderProgramCreated, Frameworks::IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.m_programName) return;
    auto program = Graphics::IGraphicAPI::Instance()->TryFindGraphicAsset<Graphics::IShaderProgramPtr>(ev->GetName());
    if (!program)
    {
        Platforms::Debug::Printf("can't get shader program asset %s", ev->GetName().c_str());
        Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuildFailed{ m_policy.m_programName, ErrorCode::findStashedAssetFail } });
        return;
    }
    m_program = Graphics::IGraphicAPI::Instance()->GetGraphicAsset<Graphics::IShaderProgramPtr>(m_policy.m_programName);
    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew ShaderProgramBuilt{ m_policy.m_programName } });
}

