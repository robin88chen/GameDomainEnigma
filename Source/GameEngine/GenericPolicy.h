/*********************************************************************
 * \file   GenericPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef GENERIC_POLICY_H
#define GENERIC_POLICY_H

#include <memory>
#include <functional>
#include "DtoDeserializer.h"

namespace Enigma::Engine
{
    class GenericDto;

    class GenericPolicy
    {
    public:
        GenericPolicy() = default;
        virtual ~GenericPolicy() = default;
        GenericPolicy(const GenericPolicy&) = default;
        GenericPolicy(GenericPolicy&&) = default;
        GenericPolicy& operator=(const GenericPolicy&) = default;
        GenericPolicy& operator=(GenericPolicy&&) = default;
    };
    using GenericPolicyConverter = std::function<std::shared_ptr<GenericPolicy>(const std::shared_ptr<IDtoDeserializer>&)>;
}

#endif // GENERIC_POLICY_H
