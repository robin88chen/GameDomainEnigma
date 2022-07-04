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

future_error IVertexShader::AsyncCompileCode(const std::string& code, const std::string& profile,
    const std::string& entry)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->CompileCode(code, profile, entry); });
}
