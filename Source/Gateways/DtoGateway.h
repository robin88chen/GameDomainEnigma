/*********************************************************************
 * \file   DtoGateway.h
 * \brief  dto gateway interface, used to map the dto (e.g. json)
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef DTO_GATEWAY_H
#define DTO_GATEWAY_H

#include "GameEngine/GenericDto.h"

namespace Enigma::Gateways
{
    class IDtoGateway
    {
    public:
        virtual ~IDtoGateway() = default;
        virtual Engine::GenericDtoCollection deserialize(const std::string& content) = 0;
        virtual std::string serialize(const Engine::GenericDtoCollection& dtos) = 0;
    };
}

#endif // DTO_GATEWAY_H
