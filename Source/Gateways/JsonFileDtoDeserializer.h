/*********************************************************************
 * \file   JsonFileDtoDeserializer.h
 * \brief  dto deserializer for json file
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _JSON_FILE_DTO_DESERIALIZER_H
#define _JSON_FILE_DTO_DESERIALIZER_H

#include <string>
#include "GameEngine/DtoDeserializer.h"

namespace Enigma::Gateways
{
    class JsonFileDtoDeserializer : public Engine::IDtoDeserializer
    {
    public:
        JsonFileDtoDeserializer();

        virtual void InvokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;
    };
}

#endif // _JSON_FILE_DTO_DESERIALIZER_H
