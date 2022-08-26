#include "IPixelShader.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

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
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), code, profile, entry, this]() 
                -> error { return CompileCode(code, profile, entry); });
    }
    else
    {
        CompileCode(code, profile, entry);
    }
}
