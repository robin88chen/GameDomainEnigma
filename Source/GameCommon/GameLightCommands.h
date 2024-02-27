/*********************************************************************
 * \file   GameLightCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef GAME_LIGHT_COMMANDS_H
#define GAME_LIGHT_COMMANDS_H

#include <string>
#include "Frameworks/Command.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include "SceneGraph/SpatialId.h"

namespace Enigma::GameCommon
{
    class CreateAmbientLight : public Frameworks::ICommand
    {
    public:
        CreateAmbientLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const MathLib::ColorRGBA& color)
            : m_parentId(parent_id), m_lightId(light_id), m_color(color) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::ColorRGBA& color() const { return m_color; }

    protected:
        SceneGraph::SpatialId m_parentId;
        SceneGraph::SpatialId m_lightId;
        MathLib::ColorRGBA m_color;
    };
    class CreateSunLight : public Frameworks::ICommand
    {
    public:
        CreateSunLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const MathLib::Vector3& dir, const MathLib::ColorRGBA& color)
            : m_parentId(parent_id), m_lightId(light_id), m_dir(dir), m_color(color) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& direction() const { return m_dir; }
        const MathLib::ColorRGBA& color() const { return m_color; }

    protected:
        SceneGraph::SpatialId m_parentId;
        SceneGraph::SpatialId m_lightId;
        MathLib::Vector3 m_dir;
        MathLib::ColorRGBA m_color;
    };
    class CreatePointLight : public Frameworks::ICommand
    {
    public:
        CreatePointLight(const SceneGraph::SpatialId& parent_id, const MathLib::Matrix4& mxLocal, const SceneGraph::SpatialId& lightId,
            const MathLib::Vector3& pos, const MathLib::ColorRGBA& color, float range)
            : m_parentId(parent_id), m_mxLocal(mxLocal), m_lightId(lightId), m_pos(pos), m_color(color), m_range(range) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const MathLib::Matrix4& localTransform() const { return m_mxLocal; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& position() const { return m_pos; }
        const MathLib::ColorRGBA& color() const { return m_color; }
        float range() const { return m_range; }

    protected:
        SceneGraph::SpatialId m_parentId;
        MathLib::Matrix4 m_mxLocal;
        SceneGraph::SpatialId m_lightId;
        MathLib::Vector3 m_pos;
        MathLib::ColorRGBA m_color;
        float m_range;
    };
    //------------------------------------------------------------------
    class ChangeLightColor : public Frameworks::ICommand
    {
    public:
        ChangeLightColor(const SceneGraph::SpatialId& light_id, const MathLib::ColorRGBA& color)
            : m_lightId(light_id), m_color(color) {}

        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::ColorRGBA& color() const { return m_color; }

    protected:
        SceneGraph::SpatialId m_lightId;
        MathLib::ColorRGBA m_color;
    };
    class ChangeLightDirection : public Frameworks::ICommand
    {
    public:
        ChangeLightDirection(const SceneGraph::SpatialId& light_id, const MathLib::Vector3& dir)
            : m_lightId(light_id), m_dir(dir) {}

        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& direction() const { return m_dir; }

    protected:
        SceneGraph::SpatialId m_lightId;
        MathLib::Vector3 m_dir;
    };
    class ChangeLightPos : public Frameworks::ICommand
    {
    public:
        ChangeLightPos(const SceneGraph::SpatialId& light_id, const MathLib::Vector3& pos)
            : m_lightId(light_id), m_pos(pos) {}
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const MathLib::Vector3& position() const { return m_pos; }

    protected:
        SceneGraph::SpatialId m_lightId;
        MathLib::Vector3 m_pos;
    };
    class ChangeLightAttenuation : public Frameworks::ICommand
    {
    public:
        ChangeLightAttenuation(const SceneGraph::SpatialId& light_id, float constant, float linear, float quadratic)
            : m_lightId(light_id), m_constant(constant), m_linear(linear), m_quadratic(quadratic) {}

        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        float constantFactor() const { return m_constant; }
        float linearFactor() const { return m_linear; }
        float quadraticFactor() const { return m_quadratic; }

    protected:
        SceneGraph::SpatialId m_lightId;
        float m_constant;
        float m_linear;
        float m_quadratic;
    };
    class ChangeLightRange : public Frameworks::ICommand
    {
    public:
        ChangeLightRange(const SceneGraph::SpatialId& light_id, float range)
            : m_lightId(light_id), m_range(range) {}
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        float range() const { return m_range; }

    protected:
        SceneGraph::SpatialId m_lightId;
        float m_range;
    };
    class EnableLight : public Frameworks::ICommand
    {
    public:
        EnableLight(const SceneGraph::SpatialId& light_id)
            : m_lightId(light_id) {}
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }

    protected:
        SceneGraph::SpatialId m_lightId;
    };
    class DisableLight : public Frameworks::ICommand
    {
    public:
        DisableLight(const SceneGraph::SpatialId& light_id)
            : m_lightId(light_id) {}
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }

    protected:
        SceneGraph::SpatialId m_lightId;
    };
    class DeleteLight : public Frameworks::ICommand
    {
    public:
        DeleteLight(const SceneGraph::SpatialId& light_id)
            : m_lightId(light_id) {}
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }

    protected:
        SceneGraph::SpatialId m_lightId;
    };
}
#endif // GAME_LIGHT_COMMANDS_H
