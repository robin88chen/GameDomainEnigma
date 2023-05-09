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
        CreateAmbientLight(const std::shared_ptr<SceneGraph::Node>& parent,
            const std::string& lightName, const MathLib::ColorRGBA& color) : m_parent(parent), m_lightName(lightName), m_color(color) {}

        std::shared_ptr<SceneGraph::Node> GetParent() const { return m_parent.lock(); }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::weak_ptr<SceneGraph::Node> m_parent;
        std::string m_lightName;
        MathLib::ColorRGBA m_color;
    };
    class CreateSunLight : public Frameworks::ICommand
    {
    public:
        CreateSunLight(const std::shared_ptr<SceneGraph::Node>& parent,
            const std::string& lightName, const MathLib::Vector3& dir, const MathLib::ColorRGBA& color)
            : m_parent(parent), m_lightName(lightName), m_dir(dir), m_color(color) {}

        std::shared_ptr<SceneGraph::Node> GetParent() const { return m_parent.lock(); }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& GetDir() const { return m_dir; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::weak_ptr<SceneGraph::Node> m_parent;
        std::string m_lightName;
        MathLib::Vector3 m_dir;
        MathLib::ColorRGBA m_color;
    };
    class CreatePointLight : public Frameworks::ICommand
    {
    public:
        CreatePointLight(const std::shared_ptr<SceneGraph::Node>& parent, const MathLib::Matrix4& mxLocal,
            const std::string& lightName, const MathLib::Vector3& pos, const MathLib::ColorRGBA& color)
            : m_parent(parent), m_mxLocal(mxLocal), m_lightName(lightName), m_pos(pos), m_color(color) {}

        std::shared_ptr<SceneGraph::Node> GetParent() const { return m_parent.lock(); }
        const MathLib::Matrix4& GetLocalTransform() const { return m_mxLocal; }
        const std::string& GetLightName() const { return m_lightName; }
        const MathLib::Vector3& GetPos() const { return m_pos; }
        const MathLib::ColorRGBA& GetColor() const { return m_color; }

    protected:
        std::weak_ptr<SceneGraph::Node> m_parent;
        MathLib::Matrix4 m_mxLocal;
        std::string m_lightName;
        MathLib::Vector3 m_pos;
        MathLib::ColorRGBA m_color;
    };
}
#endif // GAME_LIGHT_COMMANDS_H
