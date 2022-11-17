/*********************************************************************
 * \file   GeometryRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_REPOSITORY_H
#define _GEOMETRY_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/EventSubscriber.h"
#include "GeometryDataPolicy.h"
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace Enigma::Engine
{
    using error = std::error_code;

    class GeometryData;
    class Contract;
    class TriangleListContract;
    class GeometryBuilder;

    class GeometryRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GeometryRepository(Frameworks::ServiceManager* srv_manager);
        GeometryRepository(const GeometryRepository&) = delete;
        GeometryRepository(GeometryRepository&&) = delete;
        ~GeometryRepository() override;
        GeometryRepository& operator=(const GeometryRepository&) = delete;
        GeometryRepository& operator=(GeometryRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult OnInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult OnTick() override;
        /// On Term
        virtual Frameworks::ServiceResult OnTerm() override;

        bool HasGeometryData(const std::string& name);
        std::shared_ptr<GeometryData> QueryGeometryData(const std::string& name);
        std::shared_ptr<GeometryData> Create(const Contract& contract);

        error BuildGeometry(const GeometryDataPolicy& policy);

    protected:
        std::shared_ptr<GeometryData> CreateTriangleList(const TriangleListContract& contract);

        void OnGeometryBuilt(const Frameworks::IEventPtr& e);
        void OnBuildGeometryFail(const Frameworks::IEventPtr& e);

    protected:
        std::unordered_map<std::string, std::weak_ptr<GeometryData>> m_geometries;
        std::recursive_mutex m_geometryLock;

        GeometryBuilder* m_builder;
        std::queue<GeometryDataPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;

        Frameworks::EventSubscriberPtr m_onGeometryBuilt;
        Frameworks::EventSubscriberPtr m_onBuildGeometryFail;
    };
}

#endif // _GEOMETRY_REPOSITORY_H
