#include "IVertexShader.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "IShaderCompiler.h"

using namespace Enigma::Graphics;

IVertexShader::IVertexShader(const std::string& name) : m_name(name)
{
    m_hasCompiled = false;
}

IVertexShader::~IVertexShader()
{
}

void IVertexShader::Compile(const std::string& code, const std::string& profile, const std::string& entry)
{
    if (auto& compiler = IGraphicAPI::Instance()->GetShaderCompiler())
    {
        compiler->CompileVertexShader(shared_from_this(), code, profile, entry);
    }
}
