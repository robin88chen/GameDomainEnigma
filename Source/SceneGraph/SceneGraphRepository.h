/*********************************************************************
 * \file   SceneGraphRepository.h
 * \brief  scene graph objects repository, maintain objects lifecycle
 *      and some global settings
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_REPOSITORY_H
#define SCENE_GRAPH_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "SceneGraphDefines.h"
#include "Frustum.h"
#include "GameEngine/Contract.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace Enigma::SceneGraph
{
    class Camera;
    class Frustum;
    class Node;
    class Pawn;
    class LightInfo;
    class Light;

    class SceneGraphRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        SceneGraphRepository(Frameworks::ServiceManager* srv_mngr);
        SceneGraphRepository(const SceneGraphRepository&) = delete;
        SceneGraphRepository(SceneGraphRepository&&) = delete;
        virtual ~SceneGraphRepository();
        SceneGraphRepository& operator=(const SceneGraphRepository&) = delete;
        SceneGraphRepository& operator=(SceneGraphRepository&&) = delete;

        void SetCoordinateSystem(GraphicCoordSys hand);
        GraphicCoordSys GetCoordinateSystem();

        std::shared_ptr<Camera> CreateCamera(const std::string& name);
        bool HasCamera(const std::string& name);
        std::shared_ptr<Camera> QueryCamera(const std::string& name);

        std::shared_ptr<Frustum> CreateFrustum(const std::string& name, Frustum::ProjectionType proj);
        bool HasFrustum(const std::string& name);
        std::shared_ptr<Frustum> QueryFrustum(const std::string& name);

        std::shared_ptr<Node> CreateNode(const std::string& name);
        bool HasNode(const std::string& name);
        std::shared_ptr<Node> QueryNode(const std::string& name);

        std::shared_ptr<Pawn> CreatePawn(const std::string& name);
        bool HasPawn(const std::string& name);
        std::shared_ptr<Pawn> QueryPawn(const std::string& name);

        std::shared_ptr<Light> CreateLight(const std::string& name, const LightInfo& info);
        bool HasLight(const std::string& name);
        std::shared_ptr<Light> QueryLight(const std::string& name);

    private:
        void NodeContractFactory(const Engine::Contract& contract);

    private:
        GraphicCoordSys m_handSystem;

        std::unordered_map<std::string, std::weak_ptr<Camera>> m_cameras;
        std::recursive_mutex m_cameraMapLock;
        std::unordered_map<std::string, std::weak_ptr<Frustum>> m_frustums;
        std::recursive_mutex m_frustumMapLock;

        std::unordered_map<std::string, std::weak_ptr<Node>> m_nodes;
        std::recursive_mutex m_nodeMapLock;
        std::unordered_map<std::string, std::weak_ptr<Pawn>> m_pawns;
        std::recursive_mutex m_pawnMapLock;

        std::unordered_map<std::string, std::weak_ptr<Light>> m_lights;
        std::recursive_mutex m_lightMapLock;
    };
}

#endif // SCENE_GRAPH_REPOSITORY_H
