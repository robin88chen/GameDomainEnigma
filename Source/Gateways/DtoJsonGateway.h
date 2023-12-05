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

#include "DtoGateway.h"
#include <vector>
#include <string>

namespace Enigma::Gateways
{
    class DtoJsonGateway : public IDtoGateway
    {
    public:
        Engine::GenericDtoCollection deserialize(const std::string& json) override;
        std::string serialize(const Engine::GenericDtoCollection& dtos) override;
    };
}

#endif // _DTO_JSON_GATEWAY_H
