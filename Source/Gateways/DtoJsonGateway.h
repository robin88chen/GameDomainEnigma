/*********************************************************************
 * \file   DtoJsonGateway.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _DTO_JSON_GATEWAY_H
#define _DTO_JSON_GATEWAY_H

#define RAPIDJSON_HAS_STDSTRING 1

#include "GameEngine/GenericDto.h"
#include <vector>
#include <string>

namespace Enigma::Gateways
{
    class DtoJsonGateway
    {
    public:
        static Engine::GenericDtoCollection Deserialize(const std::string& json);
        static std::string Serialize(const Engine::GenericDtoCollection& contracts);
    };
}

#endif // _DTO_JSON_GATEWAY_H
