/*********************************************************************
 * \file   PrimitiveFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_FACTORY_H
#define PRIMITIVE_FACTORY_H

#include "GenericDto.h"
#include "PrimitiveId.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::Engine
{
    class Primitive;
    using PrimitiveCreator = std::function<std::shared_ptr<Primitive>(const PrimitiveId& id)>;
    using PrimitiveConstitutor = std::function<std::shared_ptr<Primitive>(const PrimitiveId& id, const Engine::GenericDto& dto)>;

    class PrimitiveFactory
    {
    public:
        PrimitiveFactory();
        ~PrimitiveFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Primitive> create(const PrimitiveId& id, const Frameworks::Rtti& rtti);
        std::shared_ptr<Primitive> constitute(const PrimitiveId& id, const Engine::GenericDto& dto, bool is_persisted);

        void registerPrimitiveFactory(const std::string& rtti, const PrimitiveCreator& creator, const PrimitiveConstitutor& constitutor);
        void unregisterPrimitiveFactory(const std::string& rtti);

    private:
        void registerPrimitiveFactory(const Frameworks::ICommandPtr& c);
        void unregisterPrimitiveFactory(const Frameworks::ICommandPtr& c);
        void createPrimitive(const Frameworks::ICommandPtr& c);
        void constitutePrimitive(const Frameworks::ICommandPtr& c);

    private:
        std::unordered_map<std::string, PrimitiveCreator> m_creators; // rtti name -> creator
        std::unordered_map<std::string, PrimitiveConstitutor> m_constitutors; // rtti name -> constitutor

        Frameworks::CommandSubscriberPtr m_registerPrimitiveFactory;
        Frameworks::CommandSubscriberPtr m_unregisterPrimitiveFactory;
        Frameworks::CommandSubscriberPtr m_createPrimitive;
        Frameworks::CommandSubscriberPtr m_constitutePrimitive;
    };
}

#endif // PRIMITIVE_FACTORY_H
