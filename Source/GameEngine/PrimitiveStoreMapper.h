/*********************************************************************
 * \file   PrimitiveStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_STORE_MAPPER_H
#define PRIMITIVE_STORE_MAPPER_H

#include "PrimitiveId.h"
#include "GenericDto.h"
#include <system_error>

namespace Enigma::Engine
{
    class PrimitiveStoreMapper
    {
    public:
        virtual ~PrimitiveStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasPrimitive(const PrimitiveId& id) = 0;
        virtual std::optional<GenericDto> queryPrimitive(const PrimitiveId& id) = 0;
        virtual std::error_code removePrimitive(const PrimitiveId& id) = 0;
        virtual std::error_code putPrimitive(const PrimitiveId& id, const GenericDto& dto) = 0;

        virtual std::uint64_t nextSequenceNumber() = 0;
    };
}

#endif // PRIMITIVE_STORE_MAPPER_H
