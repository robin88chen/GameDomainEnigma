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
#include "GameEngine/GeometryDataStoreMapper.h"
#include "GameEngine/GeometryId.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class GeometryDataFileStoreMapper : public Engine::GeometryDataStoreMapper
    {
    public:
        GeometryDataFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        ~GeometryDataFileStoreMapper() override;
        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasGeometry(const Engine::GeometryId& id) override;
        virtual std::optional<Engine::GenericDto> queryGeometry(const Engine::GeometryId& id) override;
        virtual std::error_code removeGeometry(const Engine::GeometryId& id) override;
        virtual std::error_code putGeometry(const Engine::GeometryId& id, const Engine::GenericDto& dto) override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const Engine::GeometryId& id, const Engine::FactoryDesc& factory_desc);

        std::error_code serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto);
        Engine::GenericDto deserializeDataTransferObject(const std::string& filename);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Engine::GeometryId, std::string> m_filename_map;
        std::recursive_mutex m_fileMapLock;
    };
}

#endif // GEOMETRY_DATA_FILE_STORE_MAPPER_H
