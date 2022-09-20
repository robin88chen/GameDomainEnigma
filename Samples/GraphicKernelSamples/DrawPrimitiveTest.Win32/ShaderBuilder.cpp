#include "ShaderBuilder.h"
#include <Frameworks/EventPublisher.h>
#include <Platforms/MemoryAllocMacro.h>
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IVertexShader.h"
#include "GraphicKernel/IPixelShader.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;
using namespace Enigma::Platforms;

ShaderBuilder::ShaderBuilder()
{
    m_hasVtxShaderBuilt = false;
    m_hasPixShaderBuilt = false;

    m_onVertexShaderCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexShaderCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexShaderCompiled(e); });
    EventPublisher::Subscribe(typeid(VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexLayoutCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexLayoutCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);

    m_onPixelShaderCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnPixelShaderCreated(e); });
    EventPublisher::Subscribe(typeid(DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnPixelShaderCompiled(e); });
    EventPublisher::Subscribe(typeid(PixelShaderCompiled), m_onPixelShaderCompiled);

    m_onVertexShaderBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    EventPublisher::Subscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onPixelShaderBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderBuilt(e); });
    EventPublisher::Subscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
}

ShaderBuilder::~ShaderBuilder()
{
    EventPublisher::Unsubscribe(typeid(DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(VertexShaderCompiled), m_onVertexShaderCompiled);
    m_onVertexShaderCompiled = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceVertexDeclarationCreated), m_onVertexLayoutCreated);
    m_onVertexLayoutCreated = nullptr;

    EventPublisher::Unsubscribe(typeid(DevicePixelShaderCreated), m_onPixelShaderCreated);
    m_onPixelShaderCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(PixelShaderCompiled), m_onPixelShaderCompiled);
    m_onPixelShaderCompiled = nullptr;

    EventPublisher::Unsubscribe(typeid(VertexShaderBuilt), m_onVertexShaderBuilt);
    m_onVertexShaderBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(PixelShaderBuilt), m_onPixelShaderBuilt);
    m_onPixelShaderBuilt = nullptr;
}

void ShaderBuilder::BuildShaderProgram(const ShaderProgramBuildParameter& para)
{
    m_programName = para.m_programName;
    BuildVertexShader(para.m_vtxShaderName, para.m_vtxFormatCode, para.m_vtxShaderCode, para.m_vtxLayoutName);
    BuildPixelShader(para.m_pixShaderName, para.m_pixShaderCode);
}

void ShaderBuilder::BuildVertexShader(const std::string& shader_name, const std::string& vtx_format_code,
                                      const std::string& shader_code, const std::string& layout_name)
{
    m_vtxShaderName = shader_name;
    m_vtxFormatCode = vtx_format_code;
    m_vtxShaderCode = shader_code;
    m_vtxLayoutName = layout_name;
    IGraphicAPI::Instance()->CreateVertexShader(m_vtxShaderName);
}

void ShaderBuilder::BuildPixelShader(const std::string& shader_name, const std::string& shader_code)
{
    m_pixShaderName = shader_name;
    m_pixShaderCode = shader_code;
    IGraphicAPI::Instance()->CreatePixelShader(m_pixShaderName);
}

void ShaderBuilder::OnVertexShaderCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceVertexShaderCreated, IEvent>(e);
    if (!ev) return;
    auto shader = IGraphicAPI::Instance()->TryFindGraphicAsset<IVertexShaderPtr>(ev->GetName());
    if (!shader)
    {
        Debug::Printf("can't get vertex shader asset %s", ev->GetName().c_str());
        return;
    }
    if (IGraphicAPI::Instance()->UseAsync())
    {
        shader.value()->AsyncCompileCode(m_vtxShaderCode, "vs_4_0", "vs_main");
    }
    else
    {
        shader.value()->CompileCode(m_vtxShaderCode, "vs_4_0", "vs_main");
    }
}

void ShaderBuilder::OnVertexShaderCompiled(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<VertexShaderCompiled, IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_vtxShaderName)
    {
        Debug::Printf("compiled vertex shader name not match %s", ev->GetShaderName().c_str());
        return;
    }
    auto shader = IGraphicAPI::Instance()->TryFindGraphicAsset<IVertexShaderPtr>(ev->GetShaderName());
    IGraphicAPI::Instance()->CreateVertexDeclaration(m_vtxLayoutName, m_vtxFormatCode, shader.value());
}

void ShaderBuilder::OnVertexLayoutCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceVertexDeclarationCreated, IEvent>(e);
    if (!ev) return;
    auto layout = IGraphicAPI::Instance()->TryFindGraphicAsset<IVertexDeclarationPtr>(ev->GetName());
    if (!layout)
    {
        Debug::Printf("can't get vertex layout asset %s", ev->GetName().c_str());
        return;
    }
    EventPublisher::Post(IEventPtr{ menew VertexShaderBuilt{ m_vtxShaderName } });
}

void ShaderBuilder::OnPixelShaderCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DevicePixelShaderCreated, IEvent>(e);
    if (!ev) return;
    auto shader = IGraphicAPI::Instance()->TryFindGraphicAsset<IPixelShaderPtr>(ev->GetName());
    if (!shader)
    {
        Debug::Printf("can't get pixel shader asset %s", ev->GetName().c_str());
        return;
    }
    if (IGraphicAPI::Instance()->UseAsync())
    {
        shader.value()->AsyncCompileCode(m_pixShaderCode, "ps_4_0", "ps_main");
    }
    else
    {
        shader.value()->CompileCode(m_pixShaderCode, "ps_4_0", "ps_main");
    }
}

void ShaderBuilder::OnPixelShaderCompiled(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<PixelShaderCompiled, IEvent>(e);
    if (!ev) return;
    if (ev->GetShaderName() != m_pixShaderName)
    {
        Debug::Printf("compiled pixel shader name not match %s", ev->GetShaderName().c_str());
        return;
    }
    EventPublisher::Post(IEventPtr{ menew PixelShaderBuilt{ m_pixShaderName } });
}

void ShaderBuilder::OnShaderBuilt(const IEventPtr& e)
{
    if (!e) return;
    auto ev_vtx = std::dynamic_pointer_cast<VertexShaderBuilt, IEvent>(e);
    if (ev_vtx)
    {
        if (ev_vtx->GetShaderName() != m_vtxShaderName) return;
        m_hasVtxShaderBuilt = true;
    }
    auto ev_pix = std::dynamic_pointer_cast<PixelShaderBuilt, IEvent>(e);
    if (ev_pix)
    {
        if (ev_pix->GetShaderName() != m_pixShaderName) return;
        m_hasPixShaderBuilt = true;
    }
    if ((m_hasVtxShaderBuilt) && (m_hasPixShaderBuilt))
    {
        IVertexShaderPtr vtx_shader = IGraphicAPI::Instance()->GetGraphicAsset<IVertexShaderPtr>(m_vtxShaderName);
        IPixelShaderPtr pix_shader = IGraphicAPI::Instance()->GetGraphicAsset<IPixelShaderPtr>(m_pixShaderName);
        IVertexDeclarationPtr layout = IGraphicAPI::Instance()->GetGraphicAsset<IVertexDeclarationPtr>(m_vtxLayoutName);
        if ((!vtx_shader) || (!pix_shader))
        {
            Debug::Printf("can't get vertex or pixel shader asset to create shader program");
            return;
        }
        IGraphicAPI::Instance()->CreateShaderProgram(m_programName, vtx_shader, pix_shader, layout);
    }
}
