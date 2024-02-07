/*********************************************************************
 * \file   GeometryDataFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_DATA_FACTORY_H
#define GEOMETRY_DATA_FACTORY_H

#include "GeometryId.h"
#include "GameEngine/GenericDto.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"

namespace Enigma::Geometries
{
    class GeometryData;
    using GeometryCreator = std::function<std::shared_ptr<GeometryData>(const GeometryId& id)>;
    using GeometryConstitutor = std::function<std::shared_ptr<GeometryData>(const GeometryId& id, const Engine::GenericDto& dto)>;

    class GeometryDataFactory
    {
    public:
        GeometryDataFactory();
        ~GeometryDataFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<GeometryData> create(const GeometryId& id, const Frameworks::Rtti& rtti);
        std::shared_ptr<GeometryData> constitute(const GeometryId& id, const Engine::GenericDto& dto, bool is_persisted);

        void registerGeometryFactory(const std::string& rtti_name, const GeometryCreator& creator, const GeometryConstitutor& constitutor);
        void unregisterGeometryFactory(const std::string& rtti_name);

    private:
        void registerGeometryFactory(const Frameworks::ICommandPtr& c);
        void unregisterGeometryFactory(const Frameworks::ICommandPtr& c);
        void requestGeometryCreation(const Frameworks::IQueryPtr& r);
        void requestGeometryConstitution(const Frameworks::IQueryPtr& r);

    private:
        std::unordered_map<std::string, GeometryCreator> m_creators; // rtti name -> creator
        std::unordered_map<std::string, GeometryConstitutor> m_constitutors; // rtti name -> constitutor

        Frameworks::CommandSubscriberPtr m_registerGeometryFactory;
        Frameworks::CommandSubscriberPtr m_unregisterGeometryFactory;
        Frameworks::QuerySubscriberPtr m_requestGeometryCreation;
        Frameworks::QuerySubscriberPtr m_requestGeometryConstitution;
    };
}

#endif // GEOMETRY_DATA_FACTORY_H
