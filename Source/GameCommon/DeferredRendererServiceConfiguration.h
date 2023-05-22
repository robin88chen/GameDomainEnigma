/*********************************************************************
 * \file   DeferredRendererServiceConfiguration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERER_SERVICE_CONFIGURATION_H
#define DEFERRED_RENDERER_SERVICE_CONFIGURATION_H
#include <string>

namespace Enigma::GameCommon
{
    class DeferredRendererServiceConfiguration
    {
    public:
        DeferredRendererServiceConfiguration() = default;

        const std::string& AmbientPassFxFileName() const;
        std::string& AmbientPassFxFileName();
        const std::string& AmbientEffectName() const;
        std::string& AmbientEffectName();
        const std::string& SunLightPassFxFileName() const;
        std::string& SunLightPassFxFileName();
        const std::string& LightVolumePassFxFileName() const;
        std::string& LightVolumePassFxFileName();

        const std::string& DeferredRendererTechniqueName() const;
        std::string& DeferredRendererTechniqueName();
        const std::string& GbufferTargetName() const;
        std::string& GbufferTargetName();
        const std::string& GbufferSurfaceName() const;
        std::string& GbufferSurfaceName();
        const std::string& GbufferDepthName() const;
        std::string& GbufferDepthName();

        const std::string& GbufferNormalSemantic() const;
        std::string& GbufferNormalSemantic();
        const std::string& GbufferDiffuseSemantic() const;
        std::string& GbufferDiffuseSemantic();
        const std::string& GbufferSpecularSemantic() const;
        std::string& GbufferSpecularSemantic();
        const std::string& GbufferDepthSemantic() const;
        std::string& GbufferDepthSemantic();

    protected:
        std::string m_ambientPassFxFileName;
        std::string m_ambientEffectName;
        std::string m_sunLightPassFxFileName;
        std::string m_lightVolumePassFxFileName;

        std::string m_deferredRendererTechniqueName;
        std::string m_gbufferTargetName;
        std::string m_gbufferSurfaceName;
        std::string m_gbufferDepthName;

        std::string m_gbufferNormalSemantic;
        std::string m_gbufferDiffuseSemantic;
        std::string m_gbufferSpecularSemantic;
        std::string m_gbufferDepthSemantic;
    };
}

#endif // DEFERRED_RENDERER_SERVICE_CONFIGURATION_H
