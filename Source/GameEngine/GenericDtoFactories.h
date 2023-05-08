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

        void RegisterFactory(const std::string& rtti, const DtoFactory& factory);
        void UnregisterFactory(const std::string& rtti);

        void InvokeFactory(const GenericDto& dto);

    private:
        void DoRegisteringFactory(const Frameworks::ICommandPtr& c);
        void DoUnRegisteringFactory(const Frameworks::ICommandPtr& c);
        void DoInvokingDtoFactory(const Frameworks::ICommandPtr& c);

    private:
        using DtoFactoryTable = std::unordered_map<std::string, DtoFactory>;
        DtoFactoryTable m_factories;

        Frameworks::CommandSubscriberPtr m_doRegisteringFactory;
        Frameworks::CommandSubscriberPtr m_doUnRegisteringFactory;
        Frameworks::CommandSubscriberPtr m_doInvokingDtoFactory;
    };
}

#endif // GENERIC_DTO_FACTORIES_H
