#include "IShaderVariable.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IShaderVariable::IShaderVariable(const std::string& name, const std::string& semantic)
    : m_name(name), m_semantic(semantic)
{
}

IShaderVariable::~IShaderVariable()
{
}

void IShaderVariable::Commit()
{
    if (m_commit)
    {
        m_commit(this);
    }
}

void IShaderVariable::SetCommitFunction(VariableCommitFunc fn)
{
    m_commit = fn;
}

future_error IShaderVariable::AsyncApply()
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this]() -> error { return Apply(); });
}
