/*********************************************************************
 * \file   AnimationAssetFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_FILE_STORE_MAPPER_H
#define ANIMATION_ASSET_FILE_STORE_MAPPER_H

#include "Animators/AnimationAssetStoreMapper.h"
#include "Animators/AnimationAssetId.h"
#include "Gateways/DtoGateway.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class AnimationAssetFileStoreMapper : public Animators::AnimationAssetStoreMapper
    {
    public:
        AnimationAssetFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        ~AnimationAssetFileStoreMapper() override;

        std::error_code connect() override;
        std::error_code disconnect() override;

        bool hasAnimationAsset(const Animators::AnimationAssetId& id) override;
        std::optional<Enigma::Engine::GenericDto> queryAnimationAsset(const Animators::AnimationAssetId& id) override;
        virtual std::error_code removeAnimationAsset(const Animators::AnimationAssetId& id) override;
        virtual std::error_code putAnimationAsset(const Animators::AnimationAssetId& id, const Engine::GenericDto& dto) override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const Animators::AnimationAssetId& id, const Engine::FactoryDesc& factory_desc);

        std::error_code serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto);
        Engine::GenericDto deserializeDataTransferObject(const std::string& filename);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Animators::AnimationAssetId, std::string, Animators::AnimationAssetId::hash> m_filename_map;
        std::recursive_mutex m_file_map_lock;
    };
};

#endif // ANIMATION_ASSET_FILE_STORE_MAPPER_H
