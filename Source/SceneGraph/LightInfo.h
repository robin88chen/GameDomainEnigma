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
#include <memory>

namespace Enigma::SceneGraph
{
    class LightInfoAssembler;
    class LightInfoDisassembler;

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
        LightInfo(const LightInfo&) = default;
        LightInfo(LightInfo&&) = default;
        virtual ~LightInfo() = default;
        LightInfo& operator=(const LightInfo&) = default;
        LightInfo& operator=(LightInfo&&) = default;

        std::shared_ptr<LightInfoAssembler> assembler() const;
        void assemble(const std::shared_ptr<LightInfoAssembler>& assembler) const;
        std::shared_ptr<LightInfoDisassembler> disassembler() const;
        void disassemble(const std::shared_ptr<LightInfoDisassembler>& disassembler);

        LightType lightType() const { return m_type; };

        void color(const MathLib::ColorRGBA& color);
        const MathLib::ColorRGBA& color() const { return m_color; };

        void position(const MathLib::Vector3& vec);
        const MathLib::Vector3& position() const { return m_position; };

        void direction(const MathLib::Vector3& vec);
        const MathLib::Vector3& direction() const { return m_dir; };

        void range(float range);
        float range() const { return m_range; };

        /// x,y,z : 0次,1次,2次係數
        void attenuation(const MathLib::Vector3& vecAttenuation);
        const MathLib::Vector3& attenuation() const { return m_attenuation; };

        void setEnable(bool flag);
        bool isEnable() const { return m_isEnable; };

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
