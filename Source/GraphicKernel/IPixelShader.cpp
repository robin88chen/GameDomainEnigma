#include "IPixelShader.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "IShaderCompiler.h"

using namespace Enigma::Graphics;

IPixelShader::IPixelShader(const std::string& name) : m_name(name)
{
    m_hasCompiled = false;
}

IPixelShader::~IPixelShader()
{
}

void IPixelShader::Compile(const std::string& code, const std::string& profile, const std::string& entry)
{
    if (auto& compiler = IGraphicAPI::Instance()->GetShaderCompiler())
    {
        compiler->CompilePixelShader(shared_from_this(), code, profile, entry);
    }
}
