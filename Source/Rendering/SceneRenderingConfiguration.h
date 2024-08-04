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
#include "SceneGraph/SpatialId.h"
#include <string>

namespace Enigma::Rendering
{
    class SceneRenderingConfiguration : public Renderer::IRenderingConfiguration
    {
    public:
        SceneRenderingConfiguration();

        const SceneGraph::SpatialId& primaryCameraId() const;
        void primaryCameraId(const SceneGraph::SpatialId& id);
        bool isPrimary() const;
        void isPrimary(bool primary);
        const std::string& primaryRendererTechniqueName() const;
        void primaryRendererTechniqueName(const std::string& name);

    protected:
        SceneGraph::SpatialId m_primaryCameraId;
        bool m_isPrimary;
        std::string m_primaryRendererTechniqueName;
    };

}

#endif // SCENE_RENDERING_CONFIGURATION_H
