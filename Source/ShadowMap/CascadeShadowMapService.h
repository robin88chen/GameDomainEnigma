/*********************************************************************
 * \file   CascadeShadowMapService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef CASCADE_SHADOW_MAP_SERVICE_H
#define CASCADE_SHADOW_MAP_SERVICE_H

#include "ShadowMapService.h"
#include "CascadeShadowMapServiceConfiguration.h"
#include "GraphicKernel/IDeviceRasterizerState.h"

namespace Enigma::ShadowMap
{
    class CSMSunLightCamera;

    class CascadeShadowMapService : public ShadowMapService
    {
        DECLARE_EN_RTTI;
    public:
        CascadeShadowMapService(Frameworks::ServiceManager* manager, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
            const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
            const std::shared_ptr<CascadeShadowMapServiceConfiguration>& configuration);
        virtual ~CascadeShadowMapService() override;
        CascadeShadowMapService(const CascadeShadowMapService& other) = delete;
        CascadeShadowMapService(CascadeShadowMapService&& other) noexcept = delete;
        CascadeShadowMapService& operator=(const CascadeShadowMapService& other) = delete;
        CascadeShadowMapService& operator=(CascadeShadowMapService&& other) noexcept = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        virtual void CreateShadowRenderSystem(const std::string& renderer_name, const std::string& target_name) override;
        virtual void DestroyShadowRenderSystem(const std::string& renderer_name, const std::string& target_name) override;

    protected:
        virtual void CreateSunLightCamera(const std::shared_ptr<SceneGraph::Light>& lit) override;
        virtual void DeleteSunLightCamera() override;
        virtual void UpdateSunLightDirection(const MathLib::Vector3& dir) override;

    private:
        static void AssignLightViewProjectionTransforms(Engine::EffectVariable& var);
        static void AssignCascadeDistances(Engine::EffectVariable& var);
        static void AssignCascadeTextureCoordTransforms(Engine::EffectVariable& var);
        static void AssignSliceCount(Engine::EffectVariable& var);
        static void AssignSliceDimension(Engine::EffectVariable& var);
        static void AssignFaceLightThreshold(Engine::EffectVariable& var);

    private:
        std::shared_ptr<CascadeShadowMapServiceConfiguration> m_configuration;
        std::shared_ptr<CSMSunLightCamera> m_sunLightCamera;
        //todo: backface culling 是做什麼的??
        //std::shared_ptr<Graphics::IDeviceRasterizerState> m_backfaceCullingState;
        //bool m_isRenderBackFace;

        static inline std::vector<MathLib::Matrix4> m_cascadeLightViewProjections;
        static inline std::vector<float> m_cascadeDistances;
        static inline std::vector<MathLib::Matrix4> m_cascadeTextureCoordTransforms;
        static inline float m_faceLightThreshold = 0.02f;
    };
}

#endif // CASCADE_SHADOW_MAP_SERVICE_H
