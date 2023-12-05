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

future_error IShaderVariable::AsyncApply()
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this]() -> error { return Apply(); });
}
