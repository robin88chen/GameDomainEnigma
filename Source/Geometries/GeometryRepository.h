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
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "GeometryFactoryDelegate.h"
#include "GeometryId.h"
#include <memory>
#include <mutex>
#include <unordered_map>

namespace Enigma::Geometries
{
    using error = std::error_code;

    class GeometryData;
    class GeometryDataStoreMapper;
    class GeometryDataFactory;

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
        /// On Term
        virtual Frameworks::ServiceResult onTerm() override;

        void registerGeometryFactory(const std::string& rtti_name, const GeometryCreator& creator, const GeometryConstitutor& constitutor);

        bool hasGeometryData(const GeometryId& id);
        std::shared_ptr<GeometryData> queryGeometryData(const GeometryId& id);
        void removeGeometryData(const GeometryId& id);
        void putGeometryData(const GeometryId& id, const std::shared_ptr<GeometryData>& data);
        void releaseGeometryData(const GeometryId& id);

    protected:
        void queryGeometryData(const Frameworks::IQueryPtr& q);
        void requestGeometryCreation(const Frameworks::IQueryPtr& r);
        void requestGeometryConstitution(const Frameworks::IQueryPtr& r);

        void putGeometryData(const Frameworks::ICommandPtr& c);
        void removeGeometryData(const Frameworks::ICommandPtr& c);
        void releaseGeometryData(const Frameworks::ICommandPtr& c);

    protected:
        std::shared_ptr<GeometryDataStoreMapper> m_storeMapper;
        GeometryDataFactory* m_factory;
        std::unordered_map<GeometryId, std::shared_ptr<GeometryData>, GeometryId::hash> m_geometries;
        std::recursive_mutex m_geometryLock;

        Frameworks::QuerySubscriberPtr m_queryGeometryData;
        Frameworks::QuerySubscriberPtr m_requestGeometryCreation;
        Frameworks::QuerySubscriberPtr m_requestGeometryConstitution;
        Frameworks::CommandSubscriberPtr m_putGeometryData;
        Frameworks::CommandSubscriberPtr m_removeGeometryData;
        Frameworks::CommandSubscriberPtr m_releaseGeometryData;
    };
}

#endif // _GEOMETRY_REPOSITORY_H
