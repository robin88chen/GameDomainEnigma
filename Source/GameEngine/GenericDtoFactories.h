/*********************************************************************
 * \file   GenericDtoFactories.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef GENERIC_DTO_FACTORIES_H
#define GENERIC_DTO_FACTORIES_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include <functional>
#include <unordered_map>

namespace Enigma::Engine
{
    class GenericDto;

    using DtoFactory = std::function<void(const GenericDto& dto)>;

    class GenericDtoFactories : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GenericDtoFactories(Frameworks::ServiceManager* srv_mngr);
        GenericDtoFactories(const GenericDtoFactories&) = delete;
        GenericDtoFactories(GenericDtoFactories&&) = delete;
        ~GenericDtoFactories() override;
        GenericDtoFactories& operator=(const GenericDtoFactories&) = delete;
        GenericDtoFactories& operator=(GenericDtoFactories&&) = delete;

        void registerFactory(const std::string& rtti, const DtoFactory& factory);
        void unregisterFactory(const std::string& rtti);

        void invokeFactory(const GenericDto& dto);

    private:
        void registerFactory(const Frameworks::ICommandPtr& c);
        void unregisterFactory(const Frameworks::ICommandPtr& c);
        void invokeDtoFactory(const Frameworks::ICommandPtr& c);
        void registerConverter(const Frameworks::ICommandPtr& c);
        void unregisterConverter(const Frameworks::ICommandPtr& c);

    private:
        using DtoFactoryTable = std::unordered_map<std::string, DtoFactory>;
        DtoFactoryTable m_factories;

        Frameworks::CommandSubscriberPtr m_registerFactory;
        Frameworks::CommandSubscriberPtr m_unregisterFactory;
        Frameworks::CommandSubscriberPtr m_invokeDtoFactory;
        Frameworks::CommandSubscriberPtr m_registerConverter;
        Frameworks::CommandSubscriberPtr m_unregisterConverter;
    };
}

#endif // GENERIC_DTO_FACTORIES_H
