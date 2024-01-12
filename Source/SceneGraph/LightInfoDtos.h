/*********************************************************************
 * \file   LightInfoDtos.h
 * \brief  Light Info Policy
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_INFO_DTOS_H
#define LIGHT_INFO_DTOS_H

#include "LightInfo.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include "GameEngine/GenericDto.h"
#include <optional>

namespace Enigma::SceneGraph
{
    class LightInfoDto
    {
    public:
        LightInfoDto();
        LightInfoDto(const LightInfoDto&) = default;
        LightInfoDto(LightInfoDto&&) = default;
        ~LightInfoDto() = default;
        LightInfoDto& operator=(const LightInfoDto&) = default;
        LightInfoDto& operator=(LightInfoDto&&) = default;

        LightInfo::LightType& LightType() { return m_type; }
        [[nodiscard]] LightInfo::LightType LightType() const { return m_type; }
        MathLib::ColorRGBA& Color() { return m_color; }
        [[nodiscard]] MathLib::ColorRGBA Color() const { return m_color; }
        MathLib::Vector3& Position() { return m_position; }
        [[nodiscard]] MathLib::Vector3 Position() const { return m_position; }
        MathLib::Vector3& direction() { return m_direction; }
        [[nodiscard]] MathLib::Vector3 direction() const { return m_direction; }
        float& Range() { return m_range; }
        [[nodiscard]] float Range() const { return m_range; }
        MathLib::Vector3& Attenuation() { return m_attenuation; }
        [[nodiscard]] MathLib::Vector3 Attenuation() const { return m_attenuation; }
        bool& IsEnable() { return m_isEnable; }
        [[nodiscard]] bool IsEnable() const { return m_isEnable; }

        static LightInfoDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

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

#endif // LIGHT_INFO_DTOS_H
