/*********************************************************************
 * \file   AppConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef APP_CONFIGURATION_H
#define APP_CONFIGURATION_H

#include "SceneGraph/SceneGraph.h"
#include "GameEngine/GenericDto.h"

namespace LevelEditor
{
    class AppConfiguration
    {
    public:
        AppConfiguration() = default;

        void loadConfig();

        std::string mediaPathId() const;
        //std::string dataPathId() const;
        std::string worldDataRelativePath() const;
        //std::string worldDataPathId() const;
        std::string terrainRelativePath() const;
        std::string primaryTargetName() const;
        std::string defaultRendererName() const;
        Enigma::SceneGraph::SpatialId sceneRootId() const;
        Enigma::SceneGraph::SpatialId portalManagementId() const;

        std::string worldMapRootFolderName() const;
        std::string worldMapPathId() const;

        Enigma::Engine::GenericDto cameraDto() const;
        Enigma::SceneGraph::SpatialId cameraId() const;
        std::string frustumName() const;

    private:
        Enigma::Engine::GenericDto m_configDto;
    };
}

#endif // APP_CONFIGURATION_H
