/*********************************************************************
 * \file   LightVolumePawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef LIGHT_VOLUME_PAWN_H
#define LIGHT_VOLUME_PAWN_H

#include "LightingPawn.h"

namespace Enigma::Rendering
{
    using error = std::error_code;

    class LightVolumePawn : public LightingPawn
    {
        DECLARE_EN_RTTI;
    public:
        LightVolumePawn(const SceneGraph::SpatialId& id);
        LightVolumePawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        LightVolumePawn(const LightVolumePawn&) = delete;
        LightVolumePawn(LightVolumePawn&&) = delete;
        virtual ~LightVolumePawn() override;
        LightVolumePawn& operator=(const LightVolumePawn&) = delete;
        LightVolumePawn& operator=(LightVolumePawn&&) = delete;

        static std::shared_ptr<LightVolumePawn> create(const SceneGraph::SpatialId& id);
        static std::shared_ptr<LightVolumePawn> constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto);
        virtual Engine::GenericDto serializeDto() override;

        virtual void registerHandlers() override;
        virtual void unregisterHandlers() override;

        /// 影響的光源就是自己所帶的光源
        virtual error _updateSpatialRenderState() override;

        virtual error _updateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld) override;

    protected:
        void checkBackfaceCulling();
        /// 視攝影機位置在內、外，render 內部或外部, 用 mesh select visual tech 改變
        void toggleCameraInside(bool is_inside);
        virtual void onLightInfoUpdated(const Frameworks::IEventPtr& e) override;
        void onCameraFrameChanged(const Frameworks::IEventPtr& e);

    public:
        static void setDefaultVisualTech(const std::string& token) { m_tokenDefaultVisualTech = token; }
        static void setInsideVisualTech(const std::string& token) { m_tokenInsideVisualTech = token; }
        static const std::string& getDefaultVisualTech() { return m_tokenDefaultVisualTech; }
        static const std::string& getInsideVisualTech() { return m_tokenInsideVisualTech; }

    protected:
        SceneGraph::SpatialId m_prensentCameraId;
        bool m_isCameraInside;
        static std::string m_tokenDefaultVisualTech;
        static std::string m_tokenInsideVisualTech;

        Frameworks::EventSubscriberPtr m_onCameraFrameChanged;
    };
}

#endif // LIGHT_VOLUME_PAWN_H
