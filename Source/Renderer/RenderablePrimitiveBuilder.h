/*********************************************************************
 * \file   RenderablePrimitiveBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_BUILDER_H
#define _RENDERABLE_PRIMITIVE_BUILDER_H

#include "RenderablePrimitivePolicies.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/Primitive.h"
#include "GameEngine/PrimitiveId.h"
#include "Geometries/GeometryRepository.h"
#include "GameEngine/PrimitiveRepository.h"
#include "PrimitiveBuildingPlan.h"
#include <queue>
#include <mutex>
#include <system_error>

namespace Enigma::Renderer
{
    using error = std::error_code;

    class MeshPrimitiveBuilder;
    class ModelPrimitiveBuilder;

    class RenderablePrimitiveBuilder : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        RenderablePrimitiveBuilder(Frameworks::ServiceManager* mngr, const std::shared_ptr<Engine::PrimitiveRepository>& primitive_repository, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository);
        RenderablePrimitiveBuilder(const RenderablePrimitiveBuilder&) = delete;
        RenderablePrimitiveBuilder(RenderablePrimitiveBuilder&&) = delete;
        ~RenderablePrimitiveBuilder() override;
        RenderablePrimitiveBuilder& operator=(const RenderablePrimitiveBuilder&) = delete;
        RenderablePrimitiveBuilder& operator=(RenderablePrimitiveBuilder&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        //error buildPrimitive(const Frameworks::Ruid& requester_ruid, const Engine::GenericDto& dto);

    protected:
        std::shared_ptr<Engine::Primitive> createMesh(const Engine::PrimitiveId& id);
        std::shared_ptr<Engine::Primitive> constituteMesh(const Engine::PrimitiveId& id, const Engine::GenericDto& dto);
        std::shared_ptr<Engine::Primitive> createSkinMesh(const Engine::PrimitiveId& id);
        std::shared_ptr<Engine::Primitive> constituteSkinMesh(const Engine::PrimitiveId& id, const Engine::GenericDto& dto);
        std::shared_ptr<Engine::Primitive> createModel(const Engine::PrimitiveId& id);
        std::shared_ptr<Engine::Primitive> constituteModel(const Engine::PrimitiveId& id, const Engine::GenericDto& dto);

        //void buildRenderablePrimitive(const Frameworks::Ruid& requester_ruid, const std::shared_ptr<RenderablePrimitivePolicy>& policy);
        void buildRenderablePrimitive(const PrimitiveBuildingPlan& plan);

        void onPrimitiveBuilt(const Frameworks::IEventPtr& e);
        void onBuildPrimitiveFailed(const Frameworks::IEventPtr& e);

        void buildPrimitive(const Frameworks::ICommandPtr& c);

    protected:
        std::weak_ptr<Engine::PrimitiveRepository> m_primitiveRepository;
        std::weak_ptr<Geometries::GeometryRepository> m_geometryRepository;
        //std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
        //std::queue<std::tuple<Frameworks::Ruid, std::shared_ptr<RenderablePrimitivePolicy>>> m_policies;
        std::queue<PrimitiveBuildingPlan> m_primitivePlans;
        //std::mutex m_policiesLock;
        std::mutex m_primitivePlansLock;
        //bool m_isCurrentBuilding;
        std::optional<Engine::PrimitiveId> m_currentBuildingId;
        //Frameworks::Ruid m_buildingRuid;

        Frameworks::EventSubscriberPtr m_onMeshPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildMeshPrimitiveFailed;
        //Frameworks::EventSubscriberPtr m_onModelPrimitiveBuilt;
        //Frameworks::EventSubscriberPtr m_onBuildModelPrimitiveFailed;

        Frameworks::CommandSubscriberPtr m_buildPrimitive;

        MeshPrimitiveBuilder* m_meshBuilder;
        //ModelPrimitiveBuilder* m_modelBuilder;
    };
}

#endif // _RENDERABLE_PRIMITIVE_BUILDER_H
