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

namespace Enigma::GameCommon
{
    class CreateAmbientLight : public Frameworks::ICommand
    {
    public:
        CreateAmbientLight(const std::string& parent_name, const std::string& lightName, const MathLib::ColorRGBA& color)
            : m_parentName(parent_name), m_lightName(lightName), m_color(color) {}

        const std::string& GetParentName() const { return m_parentName; }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::string m_parentName;
        std::string m_lightName;
        MathLib::ColorRGBA m_color;
    };
    class CreateSunLight : public Frameworks::ICommand
    {
    public:
        CreateSunLight(const std::string& parent_name, const std::string& lightName, const MathLib::Vector3& dir, const MathLib::ColorRGBA& color)
            : m_parentName(parent_name), m_lightName(lightName), m_dir(dir), m_color(color) {}

        const std::string& GetParentName() const { return m_parentName; }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& getDir() const { return m_dir; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::string m_parentName;
        std::string m_lightName;
        MathLib::Vector3 m_dir;
        MathLib::ColorRGBA m_color;
    };
    class CreatePointLight : public Frameworks::ICommand
    {
    public:
        CreatePointLight(const std::string& parent_name, const MathLib::Matrix4& mxLocal, const std::string& lightName,
            const MathLib::Vector3& pos, const MathLib::ColorRGBA& color, float range)
            : m_parentName(parent_name), m_mxLocal(mxLocal), m_lightName(lightName), m_pos(pos), m_color(color), m_range(range) {}

        const std::string& GetParentName() const { return m_parentName; }
        const MathLib::Matrix4& getLocalTransform() const { return m_mxLocal; }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& GetPos() const { return m_pos; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }
        float GetRange() const { return m_range; }

    protected:
        std::string m_parentName;
        MathLib::Matrix4 m_mxLocal;
        std::string m_lightName;
        MathLib::Vector3 m_pos;
        MathLib::ColorRGBA m_color;
        float m_range;
    };
    //------------------------------------------------------------------
    class ChangeLightColor : public Frameworks::ICommand
    {
    public:
        ChangeLightColor(const std::string& light_name, const MathLib::ColorRGBA& color)
            : m_lightName(light_name), m_color(color) {}

        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::string m_lightName;
        MathLib::ColorRGBA m_color;
    };
    class ChangeLightDir : public Frameworks::ICommand
    {
    public:
        ChangeLightDir(const std::string& light_name, const MathLib::Vector3& dir)
            : m_lightName(light_name), m_dir(dir) {}

        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& getDir() const { return m_dir; }

    protected:
        std::string m_lightName;
        MathLib::Vector3 m_dir;
    };
    class ChangeLightPos : public Frameworks::ICommand
    {
    public:
        ChangeLightPos(const std::string& light_name, const MathLib::Vector3& pos)
            : m_lightName(light_name), m_pos(pos) {}
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& GetPos() const { return m_pos; }

    protected:
        std::string m_lightName;
        MathLib::Vector3 m_pos;
    };
    class ChangeLightAttenuation : public Frameworks::ICommand
    {
    public:
        ChangeLightAttenuation(const std::string& light_name, float constant, float linear, float quadratic)
            : m_lightName(light_name), m_constant(constant), m_linear(linear), m_quadratic(quadratic) {}

        const std::string& GetLightName() const { return m_lightName; }
        float GetConstant() const { return m_constant; }
        float GetLinear() const { return m_linear; }
        float GetQuadratic() const { return m_quadratic; }

    protected:
        std::string m_lightName;
        float m_constant;
        float m_linear;
        float m_quadratic;
    };
    class ChangeLightRange : public Frameworks::ICommand
    {
    public:
        ChangeLightRange(const std::string& light_name, float range)
            : m_lightName(light_name), m_range(range) {}
        const std::string& GetLightName() const { return m_lightName; }
        float GetRange() const { return m_range; }

    protected:
        std::string m_lightName;
        float m_range;
    };
    class EnableLight : public Frameworks::ICommand
    {
    public:
        EnableLight(const std::string& light_name)
            : m_lightName(light_name) {}
        const std::string& GetLightName() const { return m_lightName; }

    protected:
        std::string m_lightName;
    };
    class DisableLight : public Frameworks::ICommand
    {
    public:
        DisableLight(const std::string& light_name)
            : m_lightName(light_name) {}
        const std::string& GetLightName() const { return m_lightName; }

    protected:
        std::string m_lightName;
    };
    class DeleteLight : public Frameworks::ICommand
    {
    public:
        DeleteLight(const std::string& light_name)
            : m_lightName(light_name) {}
        const std::string& GetLightName() const { return m_lightName; }

    protected:
        std::string m_lightName;
    };
}
#endif // GAME_LIGHT_COMMANDS_H
