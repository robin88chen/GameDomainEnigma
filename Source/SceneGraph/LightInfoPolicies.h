/*********************************************************************
 * \file   LightInfoPolicies.h
 * \brief  Light Info Policy
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_INFO_POLICIES_H
#define LIGHT_INFO_POLICIES_H

#include "LightInfo.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include <string>
#include <optional>

namespace Enigma::SceneGraph
{
    struct LightInfoPolicy
    {
        LightInfo::LightType m_type;
        MathLib::ColorRGBA m_color;
        std::optional<MathLib::Vector3> m_position;
        std::optional<MathLib::Vector3> m_direction;
        std::optional<float> m_range;
        std::optional<MathLib::Vector3> m_attenuation;
        bool m_isEnable;
    };
}

#endif // LIGHT_INFO_POLICIES_H
