#include "IDeviceSamplerState.h"
#include "GraphicErrors.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IDeviceSamplerState::IDeviceSamplerState(const std::string& name) : m_name(name)
{
}

IDeviceSamplerState::~IDeviceSamplerState()
{
}

error IDeviceSamplerState::CreateFromData(const SamplerStateData& data)
{
    memcpy(&m_data, &data, sizeof(SamplerStateData));
    return ErrorCode::ok;
}

future_error IDeviceSamplerState::AsyncCreateFromData(const SamplerStateData& data)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), data, this]() -> error { return CreateFromData(data); });
}
