/*********************************************************************
 * \file   GameLightCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef GAME_LIGHT_COMMANDS_H
#define GAME_LIGHT_COMMANDS_H

#include "Frameworks/Command.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include "MathLib/Matrix4.h"
#include "SceneGraph/SpatialId.h"
#include "SceneGraph/SceneGraphPersistenceLevel.h"
#include "SceneGraph/LightInfo.h"
#include "GameEngine/FactoryDesc.h"

namespace Enigma::GameCommon
{
    //------------------------------------------------------------------
    //! Architectural Decision Record (ADR)
    //! These commands are application service layer commands, so they will do completed work.
    //! for remove light service, they should execute directly.
    //------------------------------------------------------------------
    class CreateAmbientLight : public Frameworks::ICommand
    {
    public:
        CreateAmbientLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level)
            : m_parentId(parent_id), m_lightId(light_id), m_info(info), m_fd(fd), m_persistenceLevel(persistence_level) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const SceneGraph::LightInfo& info() const { return m_info; }
        const Engine::FactoryDesc& factoryDesc() const { return m_fd; }
        SceneGraph::PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

        void execute() override;

    protected:
        SceneGraph::SpatialId m_parentId;
        SceneGraph::SpatialId m_lightId;
        SceneGraph::LightInfo m_info;
        Engine::FactoryDesc m_fd;
        SceneGraph::PersistenceLevel m_persistenceLevel;
    };
    class CreateSunLight : public Frameworks::ICommand
    {
    public:
        CreateSunLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level)
            : m_parentId(parent_id), m_lightId(light_id), m_info(info), m_fd(fd), m_persistenceLevel(persistence_level) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const SceneGraph::LightInfo& info() const { return m_info; }
        const Engine::FactoryDesc& factoryDesc() const { return m_fd; }
        SceneGraph::PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

        void execute() override;

    protected:
        SceneGraph::SpatialId m_parentId;
        SceneGraph::SpatialId m_lightId;
        SceneGraph::LightInfo m_info;
        Engine::FactoryDesc m_fd;
        SceneGraph::PersistenceLevel m_persistenceLevel;
    };
    class CreatePointLight : public Frameworks::ICommand
    {
    public:
        CreatePointLight(const SceneGraph::SpatialId& parent_id, const MathLib::Matrix4& mxLocal, const SceneGraph::SpatialId& lightId, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level)
            : m_parentId(parent_id), m_mxLocal(mxLocal), m_lightId(lightId), m_info(info), m_fd(fd), m_persistenceLevel(persistence_level) {}

        const SceneGraph::SpatialId& parentId() const { return m_parentId; }
        const MathLib::Matrix4& localTransform() const { return m_mxLocal; }
        const SceneGraph::SpatialId& lightId() const { return m_lightId; }
        const SceneGraph::LightInfo& info() const { return m_info; }
        const Engine::FactoryDesc& factoryDesc() const { return m_fd; }
        SceneGraph::PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

        void execute() override;

    protected:
        SceneGraph::SpatialId m_parentId;
        MathLib::Matrix4 m_mxLocal;
        SceneGraph::SpatialId m_lightId;
        SceneGraph::LightInfo m_info;
        Engine::FactoryDesc m_fd;
        SceneGraph::PersistenceLevel m_persistenceLevel;
    };
}
#endif // GAME_LIGHT_COMMANDS_H
