#include "ShaderBuilder.h"
#include <Frameworks/EventPublisher.h>
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IVertexShader.h"

using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;

ShaderBuilder::ShaderBuilder(IGraphicAPI::AsyncType asyncType)
{
    m_async = asyncType;
    m_onVertexShaderCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexShaderCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceVertexShaderCreated), m_onVertexShaderCreated);
}

ShaderBuilder::~ShaderBuilder()
{
    EventPublisher::Unsubscribe(typeid(DeviceVertexShaderCreated), m_onVertexShaderCreated);
    m_onVertexShaderCreated = nullptr;
}

void ShaderBuilder::BuildVertexShader(const std::string& shader_name, const std::string& vtx_format_code,
    const std::string& shader_code)
{
    m_vtxShaderName = shader_name;
    m_vtxFormatCode = vtx_format_code;
    m_vtxShaderCode = shader_code;
    IGraphicAPI::Instance()->AsyncCreateVertexShader(m_vtxShaderName);
}

void ShaderBuilder::BuildPixelShader(const std::string& shader_name, const std::string& shader_code)
{
}

void ShaderBuilder::OnVertexShaderCreated(const IEventPtr& e)
{
    if (!e) return;
    DeviceVertexShaderCreated* ev = dynamic_cast<DeviceVertexShaderCreated*>(e.get());
    if (!ev) return;
    IVertexShaderPtr shader = IGraphicAPI::Instance()->GetGraphicAsset<IVertexShaderPtr>(ev->GetName());
    if (!shader) return;
    shader->AsyncCompileCode(m_vtxShaderCode, "vs_4_0", "vs_main");
}
