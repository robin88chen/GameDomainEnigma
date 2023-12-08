/*********************************************************************
 * \file   GeometryDataStoreMapper.h
 * \brief  geometry data store mapper interface, used to map the store (e.g. database)
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_DATA_STORE_MAPPER_H
#define GEOMETRY_DATA_STORE_MAPPER_H

#include "GeometryId.h"
#include "GameEngine/GenericDto.h"
#include <system_error>

namespace Enigma::Geometries
{
    class GeometryDataStoreMapper
    {
    public:
        virtual ~GeometryDataStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasGeometry(const GeometryId& id) = 0;
        virtual std::optional<Engine::GenericDto> queryGeometry(const GeometryId& id) = 0;
        virtual std::error_code removeGeometry(const GeometryId& id) = 0;
        virtual std::error_code putGeometry(const GeometryId& id, const Engine::GenericDto& dto) = 0;
    };
}


#endif // GEOMETRY_DATA_STORE_MAPPER_H
