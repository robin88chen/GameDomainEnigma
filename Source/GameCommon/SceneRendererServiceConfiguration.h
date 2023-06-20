/*********************************************************************
 * \file   SceneRendererServiceConfiguration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef SCENE_RENDERER_SERVICE_CONFIGURATION_H
#define SCENE_RENDERER_SERVICE_CONFIGURATION_H

#include <string>

namespace Enigma::GameCommon
{
    class SceneRendererServiceConfiguration
    {
    public:
        SceneRendererServiceConfiguration();

        bool IsPrimary() const;
        bool& IsPrimary();

        const std::string& PrimaryRendererTechniqueName() const;
        std::string& PrimaryRendererTechniqueName();

    protected:
        bool m_isPrimary;
        std::string m_primaryRendererTechniqueName;
    };

}

#endif // SCENE_RENDERER_SERVICE_CONFIGURATION_H
