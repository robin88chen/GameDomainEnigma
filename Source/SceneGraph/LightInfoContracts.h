/*********************************************************************
 * \file   LightInfoContracts.h
 * \brief  Light Info Policy
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_INFO_CONTRACTS_H
#define LIGHT_INFO_CONTRACTS_H

#include "LightInfo.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include "GameEngine/Contract.h"
#include <optional>

namespace Enigma::SceneGraph
{
    class LightInfoContract
    {
    public:
        LightInfoContract();
        LightInfoContract(const LightInfoContract&) = default;
        LightInfoContract(LightInfoContract&&) = default;
        ~LightInfoContract() = default;
        LightInfoContract& operator=(const LightInfoContract&) = default;
        LightInfoContract& operator=(LightInfoContract&&) = default;

        LightInfo::LightType& LightType() { return m_type; }
        [[nodiscard]] LightInfo::LightType LightType() const { return m_type; }
        MathLib::ColorRGBA& Color() { return m_color; }
        [[nodiscard]] MathLib::ColorRGBA Color() const { return m_color; }
        MathLib::Vector3& Position() { return m_position; }
        [[nodiscard]] MathLib::Vector3 Position() const { return m_position; }
        MathLib::Vector3& Direction() { return m_direction; }
        [[nodiscard]] MathLib::Vector3 Direction() const { return m_direction; }
        float& Range() { return m_range; }
        [[nodiscard]] float Range() const { return m_range; }
        MathLib::Vector3& Attenuation() { return m_attenuation; }
        [[nodiscard]] MathLib::Vector3 Attenuation() const { return m_attenuation; }
        bool& IsEnable() { return m_isEnable; }
        [[nodiscard]] bool IsEnable() const { return m_isEnable; }

        static LightInfoContract FromContract(const Engine::Contract& contract);
        Engine::Contract ToContract();

    private:
        LightInfo::LightType m_type;
        MathLib::ColorRGBA m_color;
        MathLib::Vector3 m_position;
        MathLib::Vector3 m_direction;
        float m_range;
        MathLib::Vector3 m_attenuation;
        bool m_isEnable;
    };
}

#endif // LIGHT_INFO_CONTRACTS_H
