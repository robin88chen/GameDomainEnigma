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

/*void IDeviceDepthStencilState::Create(const DepthStencilData& data)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        AsyncCreateFromData(data);
    }
    else
    {
        CreateFromData(data);
    }
}*/

void IDeviceDepthStencilState::Bind()
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        AsyncBindToDevice();
    }
    else
    {
        BindToDevice();
    }
}

error IDeviceDepthStencilState::CreateFromData(const DepthStencilData& data)
{
    memcpy(&m_data, &data, sizeof(DepthStencilData));
    return ErrorCode::ok;
}

future_error IDeviceDepthStencilState::AsyncCreateFromData(const DepthStencilData& data)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), data, this]() -> error { return CreateFromData(data); });
}

future_error IDeviceDepthStencilState::AsyncBindToDevice()
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), this]() -> error { return BindToDevice(); });
}
