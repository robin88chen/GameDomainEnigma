#include "LightInfoContracts.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_LIGHT_TYPE = "LightType";
static std::string TOKEN_LIGHT_COLOR = "LightColor";
static std::string TOKEN_LIGHT_POSITION = "LightPosition";
static std::string TOKEN_LIGHT_DIRECTION = "LightDirection";
static std::string TOKEN_LIGHT_RANGE = "LightRange";
static std::string TOKEN_LIGHT_ATTENUATION = "LightAttenuation";
static std::string TOKEN_LIGHT_ENABLE = "LightIsEnable";

LightInfoContract::LightInfoContract()
{
    m_type = LightInfo::LightType::Unknown;
    m_isEnable = false;
}

LightInfoContract LightInfoContract::FromContract(const Contract& contract)
{
    LightInfoContract light_contract;
    light_contract.LightType() = static_cast<LightInfo::LightType>(contract.TryGetValue<unsigned int>(TOKEN_LIGHT_TYPE).value());
    light_contract.Color() = contract.TryGetValue<MathLib::ColorRGBA>(TOKEN_LIGHT_COLOR).value();
    light_contract.Position() = contract.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_POSITION).value();
    light_contract.Direction() = contract.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_DIRECTION).value();
    light_contract.Range() = contract.TryGetValue<float>(TOKEN_LIGHT_RANGE).value();
    light_contract.Attenuation() = contract.TryGetValue<MathLib::Vector3>(TOKEN_LIGHT_ATTENUATION).value();
    light_contract.IsEnable() = contract.TryGetValue<bool>(TOKEN_LIGHT_ENABLE).value();
    return light_contract;
}

Contract LightInfoContract::ToContract()
{
    Contract contract;
    contract.AddOrUpdate(TOKEN_LIGHT_TYPE, static_cast<unsigned int>(m_type));
    contract.AddOrUpdate(TOKEN_LIGHT_COLOR, m_color);
    contract.AddOrUpdate(TOKEN_LIGHT_POSITION, m_position);
    contract.AddOrUpdate(TOKEN_LIGHT_DIRECTION, m_direction);
    contract.AddOrUpdate(TOKEN_LIGHT_RANGE, m_range);
    contract.AddOrUpdate(TOKEN_LIGHT_ATTENUATION, m_attenuation);
    contract.AddOrUpdate(TOKEN_LIGHT_ENABLE, m_isEnable);
    return contract;
}
