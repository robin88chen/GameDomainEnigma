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
        void CreateFromDto(const std::string& name, const GenericDto& dto);

        void OnDtoDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializeDtoFailed(const Frameworks::IEventPtr& e);
        void OnDtoGeometryCreated(const Frameworks::IEventPtr& e);

    protected:
        GeometryRepository* m_hostRepository;
        Frameworks::Ruid m_ruidDeserializing;
        Frameworks::Ruid m_ruidContracting;
        GeometryDataPolicy m_policy;

        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeDtoFailed;
        Frameworks::EventSubscriberPtr m_onDtoGeometryCreated;
    };
}

#endif // _GEOMETRY_BUILDER_H
