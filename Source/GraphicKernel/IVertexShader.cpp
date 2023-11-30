#include "IVertexShader.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

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
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), code, profile, entry, this]() 
                -> error { return CompileCode(code, profile, entry); });
    }
    else
    {
        CompileCode(code, profile, entry);
    }
}
