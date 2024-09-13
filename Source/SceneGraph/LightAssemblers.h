/*****************************************************************
 * \file   LightAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef LIGHT_ASSEMBLERS_H
#define LIGHT_ASSEMBLERS_H

#include "LightInfo.h"
#include "SpatialId.h"
#include "SpatialAssembler.h"

namespace Enigma::SceneGraph
{
    class Light;

    class LightInfoAssembler
    {
    public:
        LightInfoAssembler(const LightInfo::LightType& type);

        [[nodiscard]] LightInfo::LightType lightType() const { return m_type; }

        void color(const MathLib::ColorRGBA& color) { m_color = color; }
        void position(const MathLib::Vector3& position);
        void direction(const MathLib::Vector3& direction);
        void range(float range);
        void attenuation(const MathLib::Vector3& attenuation);
        void isEnable(bool is_enable) { m_isEnable = is_enable; }

        Engine::GenericDto assemble() const;

    private:
        LightInfo::LightType m_type;
        MathLib::ColorRGBA m_color;
        MathLib::Vector3 m_position;
        MathLib::Vector3 m_direction;
        float m_range;
        MathLib::Vector3 m_attenuation;
        bool m_isEnable;
    };

    class LightInfoDisassembler
    {
    public:
        LightInfoDisassembler();

        [[nodiscard]] LightInfo::LightType lightType() const { return m_type; }
        [[nodiscard]] MathLib::ColorRGBA color() const { return m_color; }
        [[nodiscard]] MathLib::Vector3 position() const { return m_position; }
        [[nodiscard]] MathLib::Vector3 direction() const { return m_direction; }
        [[nodiscard]] float range() const { return m_range; }
        [[nodiscard]] MathLib::Vector3 attenuation() const { return m_attenuation; }
        [[nodiscard]] bool isEnable() const { return m_isEnable; }

        void disassemble(const Engine::GenericDto& dto);

    private:
        LightInfo::LightType m_type;
        MathLib::ColorRGBA m_color;
        MathLib::Vector3 m_position;
        MathLib::Vector3 m_direction;
        float m_range;
        MathLib::Vector3 m_attenuation;
        bool m_isEnable;
    };

    class LightAssembler : public SpatialAssembler
    {
    public:
        LightAssembler(const SpatialId& id, const LightInfo::LightType& type);

        void color(const MathLib::ColorRGBA& color);
        void position(const MathLib::Vector3& position);
        void direction(const MathLib::Vector3& direction);
        void range(float range);
        void attenuation(const MathLib::Vector3& attenuation);
        void isEnable(bool is_enable);
        void lightInfo(const LightInfo& info);
        void lightInfo(const std::shared_ptr<LightInfoAssembler>& info_assembler);

        Engine::GenericDto assemble() const;

    private:
        std::shared_ptr<LightInfoAssembler> m_infoAssembler;
    };
    class LightDisassembler : public SpatialDisassembler
    {
    public:
        LightDisassembler();

        [[nodiscard]] LightInfo lightInfo() const { return m_info; }

        void disassemble(const Engine::GenericDto& dto);

    private:
        LightInfo m_info;
    };
}

#endif // LIGHT_ASSEMBLERS_H
