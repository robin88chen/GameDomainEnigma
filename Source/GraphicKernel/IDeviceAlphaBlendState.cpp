#include "IDeviceAlphaBlendState.h"
#include "GraphicErrors.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IDeviceAlphaBlendState::IDeviceAlphaBlendState(const std::string& name)
{
    m_name = name;
}

IDeviceAlphaBlendState::~IDeviceAlphaBlendState()
{
}

/*void IDeviceAlphaBlendState::create(const BlendStateData& data)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        AsyncCreateFromData(data);
    }
    else
    {
        CreateFromData(data);
    }
}*/

void IDeviceAlphaBlendState::Bind()
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        AsyncBindToDevice();
    }
    else
    {
        BindToDevice();
    }
}

error IDeviceAlphaBlendState::CreateFromData(const BlendStateData& data)
{
    memcpy(&m_data, &data, sizeof(BlendStateData));
    return ErrorCode::ok;
}

future_error IDeviceAlphaBlendState::AsyncCreateFromData(const BlendStateData& data)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), data, this]() -> error { return CreateFromData(data); });
}

future_error IDeviceAlphaBlendState::AsyncBindToDevice()
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this]() -> error { return BindToDevice(); });
}
