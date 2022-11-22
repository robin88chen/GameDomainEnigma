/*********************************************************************
 * \file   GeometryBuilder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_BUILDER_H
#define _GEOMETRY_BUILDER_H

#include "Contract.h"
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
        void CreateFromContract(const std::string& name, const Contract& contract);

        void OnContractDeserialized(const Frameworks::IEventPtr& e);
        void OnContractedGeometryCreated(const Frameworks::IEventPtr& e);

    protected:
        GeometryRepository* m_hostRepository;
        Frameworks::Ruid m_ruidDeserializing;
        Frameworks::Ruid m_ruidContracting;
        GeometryDataPolicy m_policy;

        Frameworks::EventSubscriberPtr m_onContractDeserialized;
        Frameworks::EventSubscriberPtr m_onContractedGeometryCreated;
    };
}

#endif // _GEOMETRY_BUILDER_H
