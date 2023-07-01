/*********************************************************************
 * \file   TerrainPrimitiveBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_BUILDER_H
#define TERRAIN_PRIMITIVE_BUILDER_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/ruid.h"
#include "Frameworks/RequestSubscriber.h"
#include <system_error>
#include <mutex>
#include <queue>

namespace Enigma::Terrain
{
    using error = std::error_code;

    class TerrainPrimitivePolicy;

    class TerrainPrimitiveBuilder : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TerrainPrimitiveBuilder(Frameworks::ServiceManager* mngr);
        TerrainPrimitiveBuilder(const TerrainPrimitiveBuilder&) = delete;
        TerrainPrimitiveBuilder(TerrainPrimitiveBuilder&&) = delete;
        TerrainPrimitiveBuilder& operator=(const TerrainPrimitiveBuilder&) = delete;
        TerrainPrimitiveBuilder& operator=(TerrainPrimitiveBuilder&&) = delete;
        ~TerrainPrimitiveBuilder();

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        error BuildPrimitive(const Frameworks::Ruid& requester_ruid, const std::shared_ptr<TerrainPrimitivePolicy>& policy);

    protected:
        void DoBuildingPrimitive(const Frameworks::IRequestPtr& r);

        void BuildTerrainPrimitive(const Frameworks::Ruid& requester_ruid, const std::shared_ptr<TerrainPrimitivePolicy>& policy);

    protected:
        std::queue<std::tuple<Frameworks::Ruid, std::shared_ptr<TerrainPrimitivePolicy>>> m_policies;
        std::mutex m_policiesLock;
        bool m_isCurrentBuilding;
        Frameworks::Ruid m_buildingRuid;

        Frameworks::RequestSubscriberPtr m_doBuildingPrimitive;
    };
}

#endif // TERRAIN_PRIMITIVE_BUILDER_H
