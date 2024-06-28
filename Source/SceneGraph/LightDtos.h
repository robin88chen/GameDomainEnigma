/*********************************************************************
 * \file   LightDtos.h
 * \brief  Light Info Policy
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_DTOS_H
#define LIGHT_DTOS_H

#include "LightInfo.h"
#include "SpatialDto.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
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

        void lightType(LightInfo::LightType type) { m_type = type; }
        [[nodiscard]] LightInfo::LightType lightType() const { return m_type; }
        void color(const MathLib::ColorRGBA& c) { m_color = c; }
        [[nodiscard]] MathLib::ColorRGBA color() const { return m_color; }
        void position(const MathLib::Vector3& pos) { m_position = pos; }
        [[nodiscard]] MathLib::Vector3 position() const { return m_position; }
        void direction(const MathLib::Vector3& dir) { m_direction = dir; }
        [[nodiscard]] MathLib::Vector3 direction() const { return m_direction; }
        void range(float r) { m_range = r; }
        [[nodiscard]] float range() const { return m_range; }
        void attenuation(const MathLib::Vector3& att) { m_attenuation = att; }
        [[nodiscard]] MathLib::Vector3 attenuation() const { return m_attenuation; }
        void isEnable(bool enable) { m_isEnable = enable; }
        [[nodiscard]] bool isEnable() const { return m_isEnable; }

        static LightInfoDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    private:
        LightInfo::LightType m_type;
        MathLib::ColorRGBA m_color;
        MathLib::Vector3 m_position;
        MathLib::Vector3 m_direction;
        float m_range;
        MathLib::Vector3 m_attenuation;
        bool m_isEnable;
    };
    class LightDto : public SpatialDto
    {
    public:
        LightDto();
        LightDto(const SpatialDto& spatial_dto);
        LightDto(const Engine::GenericDto& dto);

        [[nodiscard]] Engine::GenericDto lightInfo() const { return m_lightInfo; }
        void lightInfo(const Engine::GenericDto& dto) { m_lightInfo = dto; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::GenericDto m_lightInfo;
    };
}

#endif // LIGHT_INFO_DTOS_H
