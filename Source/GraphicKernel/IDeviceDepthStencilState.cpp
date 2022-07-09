#include "IDeviceDepthStencilState.h"
#include "GraphicErrors.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IDeviceDepthStencilState::IDeviceDepthStencilState(const std::string& name)
{
    m_name = name;
}

IDeviceDepthStencilState::~IDeviceDepthStencilState()
{
}

error IDeviceDepthStencilState::CreateFromData(const DepthStencilData& data)
{
    memcpy(&m_data, &data, sizeof(DepthStencilData));
    return ErrorCode::ok;
}

future_error IDeviceDepthStencilState::AsyncCreateFromData(const DepthStencilData& data)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->CreateFromData(data); });
}

future_error IDeviceDepthStencilState::AsyncBindToDevice()
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->BindToDevice(); });
}
