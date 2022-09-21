#include "AsyncShaderCompilerDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/GraphicThread.h"

using namespace Enigma::Graphics;
using namespace Enigma::Devices;

AsyncShaderCompilerDx11::AsyncShaderCompilerDx11(ID3D11Device* device) : ShaderCompilerDx11(device)
{
    
}

AsyncShaderCompilerDx11::~AsyncShaderCompilerDx11()
{
    
}

error AsyncShaderCompilerDx11::CompileVertexShader(const Graphics::IVertexShaderPtr& shader,
    const std::string& code, const std::string& profile, const std::string& entry)
{
    IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return ShaderCompilerDx11::CompileVertexShader(shader, code, profile, entry); });
    return ErrorCode::ok;
}

error AsyncShaderCompilerDx11::CompilePixelShader(const Graphics::IPixelShaderPtr& shader,
    const std::string& code, const std::string& profile, const std::string& entry)
{
    IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return ShaderCompilerDx11::CompilePixelShader(shader, code, profile, entry); });
    return ErrorCode::ok;
}
