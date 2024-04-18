/*********************************************************************
 * \file   GeometryDataFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_DATA_FILE_STORE_MAPPER_H
#define GEOMETRY_DATA_FILE_STORE_MAPPER_H

#include "GameEngine/FactoryDesc.h"
#include "Gateways/DtoGateway.h"
#include "Geometries/GeometryDataStoreMapper.h"
#include "Geometries/GeometryId.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class GeometryDataFileStoreMapper : public Geometries::GeometryDataStoreMapper
    {
    public:
        GeometryDataFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        ~GeometryDataFileStoreMapper() override;
        GeometryDataFileStoreMapper(const GeometryDataFileStoreMapper&) = delete;
        GeometryDataFileStoreMapper(GeometryDataFileStoreMapper&&) = delete;
        GeometryDataFileStoreMapper& operator=(const GeometryDataFileStoreMapper&) = delete;
        GeometryDataFileStoreMapper& operator=(GeometryDataFileStoreMapper&&) = delete;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasGeometry(const Geometries::GeometryId& id) override;
        virtual std::optional<Engine::GenericDto> queryGeometry(const Geometries::GeometryId& id) override;
        virtual std::error_code removeGeometry(const Geometries::GeometryId& id) override;
        virtual std::error_code putGeometry(const Geometries::GeometryId& id, const Engine::GenericDto& dto) override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const Geometries::GeometryId& id, const Engine::FactoryDesc& factory_desc);

        std::error_code serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto);
        Engine::GenericDto deserializeDataTransferObject(const std::string& filename);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Geometries::GeometryId, std::string, Geometries::GeometryId::hash> m_filename_map;
        mutable std::recursive_mutex m_fileMapLock;
    };
}

#endif // GEOMETRY_DATA_FILE_STORE_MAPPER_H
