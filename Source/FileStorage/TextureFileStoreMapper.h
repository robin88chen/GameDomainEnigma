/*********************************************************************
 * \file   TextureFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef TEXTURE_FILE_STORE_MAPPER_H
#define TEXTURE_FILE_STORE_MAPPER_H

#include "GameEngine/TextureId.h"
#include "Gateways/DtoGateway.h"
#include "GameEngine/TextureStoreMapper.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class TextureFileStoreMapper : public Engine::TextureStoreMapper
    {
    public:
        TextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        TextureFileStoreMapper(const TextureFileStoreMapper& other) = delete;
        TextureFileStoreMapper(TextureFileStoreMapper&& other) noexcept = delete;
        ~TextureFileStoreMapper() override;
        TextureFileStoreMapper& operator=(const TextureFileStoreMapper& other) = delete;
        TextureFileStoreMapper& operator=(TextureFileStoreMapper&& other) noexcept = delete;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasTexture(const Engine::TextureId& id) override;
        virtual std::optional<Engine::GenericDto> queryTexture(const Engine::TextureId& id) override;
        virtual std::error_code removeTexture(const Engine::TextureId& id) override;
        virtual std::error_code putTexture(const Engine::TextureId& id, const Engine::GenericDto& dto) override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::error_code serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto);
        Engine::GenericDto deserializeDataTransferObject(const std::string& filename);
        std::string extractFilename(const Engine::TextureId& id, const Engine::FactoryDesc& factory_desc);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Engine::TextureId, std::string, Engine::TextureId::hash> m_filename_map;
        std::recursive_mutex m_fileMapLock;
    };
}

#endif // TEXTURE_FILE_STORE_MAPPER_H
