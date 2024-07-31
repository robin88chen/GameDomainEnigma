/*****************************************************************
 * \file   LightCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 ******************************************************************/
#ifndef LIGHT_COMMANDS_H
#define LIGHT_COMMANDS_H

#include "Frameworks/Command.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include "SpatialId.h"

namespace Enigma::SceneGraph
{
    class ChangeLightColor : public Frameworks::ICommand
    {
    public:
        ChangeLightColor(const SpatialId& light_id, const MathLib::ColorRGBA& color)
            : m_lightId(light_id), m_color(color) {}

        const SpatialId& lightId() const { return m_lightId; }
        const MathLib::ColorRGBA& color() const { return m_color; }

        void execute() override;

    protected:
        SpatialId m_lightId;
        MathLib::ColorRGBA m_color;
    };
    class ChangeLightDirection : public Frameworks::ICommand
    {
    public:
        ChangeLightDirection(const SpatialId& light_id, const MathLib::Vector3& dir)
            : m_lightId(light_id), m_dir(dir) {}

        const SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& direction() const { return m_dir; }

        void execute() override;

    protected:
        SpatialId m_lightId;
        MathLib::Vector3 m_dir;
    };
    class ChangeLightPos : public Frameworks::ICommand
    {
    public:
        ChangeLightPos(const SpatialId& light_id, const MathLib::Vector3& pos)
            : m_lightId(light_id), m_pos(pos) {}
        const SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& position() const { return m_pos; }

        void execute() override;

    protected:
        SpatialId m_lightId;
        MathLib::Vector3 m_pos;
    };
    class ChangeLightAttenuation : public Frameworks::ICommand
    {
    public:
        ChangeLightAttenuation(const SpatialId& light_id, float constant, float linear, float quadratic)
            : m_lightId(light_id), m_constant(constant), m_linear(linear), m_quadratic(quadratic) {}

        const SpatialId& lightId() const { return m_lightId; }
        float constantFactor() const { return m_constant; }
        float linearFactor() const { return m_linear; }
        float quadraticFactor() const { return m_quadratic; }

        void execute() override;

    protected:
        SpatialId m_lightId;
        float m_constant;
        float m_linear;
        float m_quadratic;
    };
    class ChangeLightRange : public Frameworks::ICommand
    {
    public:
        ChangeLightRange(const SpatialId& light_id, float range)
            : m_lightId(light_id), m_range(range) {}
        const SpatialId& lightId() const { return m_lightId; }
        float range() const { return m_range; }

        void execute() override;

    protected:
        SpatialId m_lightId;
        float m_range;
    };
    class EnableLight : public Frameworks::ICommand
    {
    public:
        EnableLight(const SpatialId& light_id)
            : m_lightId(light_id) {}
        const SpatialId& lightId() const { return m_lightId; }

        void execute() override;

    protected:
        SpatialId m_lightId;
    };
    class DisableLight : public Frameworks::ICommand
    {
    public:
        DisableLight(const SpatialId& light_id)
            : m_lightId(light_id) {}
        const SpatialId& lightId() const { return m_lightId; }

        void execute() override;

    protected:
        SpatialId m_lightId;
    };
    class DeleteLight : public Frameworks::ICommand
    {
    public:
        DeleteLight(const SpatialId& light_id)
            : m_lightId(light_id) {}
        const SpatialId& lightId() const { return m_lightId; }

        void execute() override;

    protected:
        SpatialId m_lightId;
    };
}

#endif // LIGHT_COMMANDS_H
