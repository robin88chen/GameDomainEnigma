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
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "GeometryId.h"
#include "GeometryDataPolicy.h"
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace Enigma::Engine
{
    using error = std::error_code;

    class GeometryData;
    class GenericDto;
    class GeometryDataStoreMapper;
    class GeometryDataFactory;
    class TriangleListDto;
    class GeometryBuilder;

    class GeometryRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GeometryRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<GeometryDataStoreMapper>& store_mapper);
        GeometryRepository(const GeometryRepository&) = delete;
        GeometryRepository(GeometryRepository&&) = delete;
        ~GeometryRepository() override;
        GeometryRepository& operator=(const GeometryRepository&) = delete;
        GeometryRepository& operator=(GeometryRepository&&) = delete;

        /// On Init
        virtual Frameworks::ServiceResult onInit() override;
        /// On Tick
        virtual Frameworks::ServiceResult onTick() override;
        /// On Term
        virtual Frameworks::ServiceResult onTerm() override;

        GeometryDataFactory* factory() { return m_factory; }

        bool hasGeometryData(const GeometryId& id);
        std::shared_ptr<GeometryData> queryGeometryData(const GeometryId& id);
        void removeGeometryData(const GeometryId& id);
        void putGeometryData(const GeometryId& id, const std::shared_ptr<GeometryData>& data);

        //error BuildGeometry(const GeometryDataPolicy& policy);

    protected:
        //void OnFactoryGeometryCreated(const Frameworks::IEventPtr& e);
        //void OnGeometryBuilt(const Frameworks::IEventPtr& e);
        //void OnBuildGeometryFail(const Frameworks::IEventPtr& e);
        //void DoBuildingGeometry(const Frameworks::ICommandPtr& c);

        void queryGeometryData(const Frameworks::IQueryPtr& q);

    protected:
        std::shared_ptr<GeometryDataStoreMapper> m_storeMapper;
        GeometryDataFactory* m_factory;
        std::unordered_map<GeometryId, std::shared_ptr<GeometryData>, GeometryId::hash> m_geometries;
        std::recursive_mutex m_geometryLock;

        GeometryBuilder* m_builder;
        std::queue<GeometryDataPolicy> m_policies;
        bool m_isCurrentBuilding;
        std::mutex m_policiesLock;

        //Frameworks::EventSubscriberPtr m_onGeometryBuilt;
        //Frameworks::EventSubscriberPtr m_onBuildGeometryFail;
        //Frameworks::CommandSubscriberPtr m_doBuildingGeometry;

        Frameworks::QuerySubscriberPtr m_queryGeometryData;
    };
}

#endif // _GEOMETRY_REPOSITORY_H
