/*********************************************************************
 * \file   AsyncJsonFileDtoDeserializer.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _ASYNC_JSON_FILE_DTO_DESERIALIZER_H
#define _ASYNC_JSON_FILE_DTO_DESERIALIZER_H

#include <string>
#include <future>
#include "GameEngine/DtoDeserializer.h"

namespace Enigma::Gateways
{
    class IDtoGateway;
    class AsyncJsonFileDtoDeserializer : public Engine::IDtoDeserializer
    {
    public:
        AsyncJsonFileDtoDeserializer();

        virtual void invokeDeserialize(const Frameworks::Ruid& ruid_deserializing, const std::string& param) override;
    protected:
        void DeserializeProcedure();

    protected:
        std::shared_ptr<IDtoGateway> m_gateway;
        Frameworks::Ruid m_ruid;
        std::string m_parameter;
        std::future<void> m_deserializing;
    };
}

#endif // _ASYNC_JSON_FILE_CONTRACT_DESERIALIZER_H
