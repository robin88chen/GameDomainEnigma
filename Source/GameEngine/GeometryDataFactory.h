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
#include "GenericDto.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::Engine
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
        std::shared_ptr<GeometryData> constitute(const GeometryId& id, const GenericDto& dto);

    private:
        void registerGeometryFactory(const Frameworks::ICommandPtr& c);
        void unregisterGeometryFactory(const Frameworks::ICommandPtr& c);
        void createGeometry(const Frameworks::ICommandPtr& c);
        void constituteGeometry(const Frameworks::ICommandPtr& c);

    private:
        std::unordered_map<std::string, GeometryCreator> m_creators; // rtti name -> creator
        std::unordered_map<std::string, GeometryConstitutor> m_constitutors; // rtti name -> constitutor

        Frameworks::CommandSubscriberPtr m_registerGeometryFactory;
        Frameworks::CommandSubscriberPtr m_unregisterGeometryFactory;
        Frameworks::CommandSubscriberPtr m_createGeometry;
        Frameworks::CommandSubscriberPtr m_constituteGeometry;
    };
}

#endif // GEOMETRY_DATA_FACTORY_H
