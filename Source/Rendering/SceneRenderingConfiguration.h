/*********************************************************************
 * \file   SceneRenderingConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef SCENE_RENDERING_CONFIGURATION_H
#define SCENE_RENDERING_CONFIGURATION_H

#include "Renderer/RenderingConfiguration.h"
#include <string>

namespace Enigma::Rendering
{
    class SceneRenderingConfiguration : public Renderer::IRenderingConfiguration
    {
    public:
        SceneRenderingConfiguration();

        bool isPrimary() const;
        void isPrimary(bool primary);

        const std::string& primaryRendererTechniqueName() const;
        void primaryRendererTechniqueName(const std::string& name);

    protected:
        bool m_isPrimary;
        std::string m_primaryRendererTechniqueName;
    };

}

#endif // SCENE_RENDERING_CONFIGURATION_H
