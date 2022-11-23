/*********************************************************************
 * \file   LightInfo.h
 * \brief  Light Info, a data transfer object, owned by light node
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _LIGHT_INFO_H
#define _LIGHT_INFO_H

#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"

namespace Enigma::SceneGraph
{
    class LightInfoDto;

    class LightInfo
    {
    public:
        /** 定義光源形式，已定義型式的幾種光，會與effect fx中semantic對應，
        其他的就custom設定了
        */
        enum class LightType
        {
            Unknown = 0,
            Ambient,            ///< for ambient light
            SunLight,           ///< for sun light (directional)
            Point,              ///< for custom point light
            Directional,    ///< for custom directional light
        };

    public:
        LightInfo();
        LightInfo(LightType type);
        LightInfo(const LightInfoDto& dto);
        LightInfo(const LightInfo&) = default;
        LightInfo(LightInfo&&) = default;
        virtual ~LightInfo() = default;
        LightInfo& operator=(const LightInfo&) = default;
        LightInfo& operator=(LightInfo&&) = default;

        LightInfoDto SerializeDto();

        LightType GetLightType() const { return m_type; };

        void SetLightColor(const MathLib::ColorRGBA& color);
        const MathLib::ColorRGBA& GetLightColor() const { return m_color; };

        void SetLightPosition(const MathLib::Vector3& vec);
        const MathLib::Vector3& GetLightPosition() const { return m_position; };

        void SetLightDirection(const MathLib::Vector3& vec);
        const MathLib::Vector3& GetLightDirection() const { return m_dir; };

        void SetLightRange(float range);
        float GetLightRange() const { return m_range; };

        /// x,y,z : 0次,1次,2次係數
        void SetLightAttenuation(const MathLib::Vector3& vecAttenuation);
        const MathLib::Vector3& GetLightAttenuation() const { return m_attenuation; };

        void SetEnable(bool flag);
        bool IsEnable() const { return m_isEnable; };

    protected:
        LightType m_type;

        MathLib::ColorRGBA m_color;

        MathLib::Vector3 m_dir;
        MathLib::Vector3 m_position;

        float m_range;  ///< default : 10
        MathLib::Vector3 m_attenuation;  ///< x,y,z : 0次,1次,2次係數, default (0,1,0)

        bool m_isEnable;
    };
}

#endif // _LIGHT_INFO_H
