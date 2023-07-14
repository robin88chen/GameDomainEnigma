/*********************************************************************
 * \file   AppConfiguration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef APP_CONFIGURATION_H
#define APP_CONFIGURATION_H

#include "GameEngine/GenericDto.h"

namespace LevelEditor
{
    class AppConfiguration
    {
    public:
        AppConfiguration() = default;

        void LoadConfig();

        std::string GetMediaPathId() const;
        std::string GetDataPathId() const;
        std::string GetPrimaryTargetName() const;
        std::string GetDefaultRendererName() const;
        std::string GetSceneRootName() const;
        std::string GetPortalManagementName() const;

        std::string GetWorldMapRootFolderName() const;
        std::string GetWorldMapPathId() const;

        Enigma::Engine::GenericDto GetCameraDto() const;
    private:
        Enigma::Engine::GenericDto m_configDto;
    };
}

#endif // APP_CONFIGURATION_H
