/*********************************************************************
 * \file   GeometryBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_BUILDER_H
#define _GEOMETRY_BUILDER_H

#include "GenericDto.h"
#include "GeometryDataPolicy.h"
#include "Frameworks/ruid.h"
#include "Frameworks/EventSubscriber.h"
#include "GeometryCommands.h"
#include "Frameworks/CommandSubscriber.h"
#include <string>
#include <unordered_map>

namespace Enigma::Engine
{
    class GeometryRepository;

    class GeometryBuilder
    {
    public:
        GeometryBuilder(GeometryRepository* host);
        GeometryBuilder(const GeometryBuilder&) = delete;
        GeometryBuilder(GeometryBuilder&&) = delete;
        ~GeometryBuilder();
        GeometryBuilder& operator=(const GeometryBuilder&) = delete;
        GeometryBuilder& operator=(GeometryBuilder&&) = delete;

        void BuildGeometry(const GeometryDataPolicy& policy);

    protected:
        void CreateFromDto(const GeometryId& id, const GenericDto& dto);
        void GeometryFactory(const GeometryId& id, const Engine::GenericDto& dto);

        void OnDtoDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializeDtoFailed(const Frameworks::IEventPtr& e);
        void OnFactoryGeometryCreated(const Frameworks::IEventPtr& e);

        void DoRegisteringGeometryFactory(const Frameworks::ICommandPtr& c);
        void DoUnRegisteringGeometryFactory(const Frameworks::ICommandPtr& c);
    protected:
        GeometryRepository* m_hostRepository;
        Frameworks::Ruid m_ruidDeserializing;
        Frameworks::Ruid m_ruidInstancing;
        GeometryDataPolicy m_policy;

        //using DtoFactoryTable = std::unordered_map<std::string, GeometryDtoFactory>;
        //DtoFactoryTable m_factories;

        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeDtoFailed;
        Frameworks::EventSubscriberPtr m_onFactoryGeometryCreated;
        Frameworks::CommandSubscriberPtr m_doRegisteringGeometryFactory;
        Frameworks::CommandSubscriberPtr m_doUnRegisteringGeometryFactory;
    };
}

#endif // _GEOMETRY_BUILDER_H
