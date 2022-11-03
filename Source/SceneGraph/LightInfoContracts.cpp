#include "LightInfoContracts.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

std::string TOKEN_LIGHT_TYPE = "LightType";
std::string TOKEN_LIGHT_COLOR = "LightColor";
std::string TOKEN_LIGHT_POSITION = "LightPosition";
std::string TOKEN_LIGHT_DIRECTION = "LightDirection";
std::string TOKEN_LIGHT_RANGE = "LightRange";
std::string TOKEN_LIGHT_ATTENUATION = "LightAttenuation";
std::string TOKEN_LIGHT_ENABLE = "LightIsEnable";

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
