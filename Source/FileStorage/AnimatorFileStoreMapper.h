/*********************************************************************
 * \file   AnimatorFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_FILE_STORE_MAPPER_H
#define ANIMATOR_FILE_STORE_MAPPER_H

#include "GameEngine/GenericDto.h"
#include "Gateways/DtoGateway.h"
#include "Animators/AnimatorStoreMapper.h"
#include "Animators/AnimatorId.h"
#include <mutex>

namespace Enigma::FileStorage
{
    class AnimatorFileStoreMapper : public Animators::AnimatorStoreMapper
    {
    public:
        AnimatorFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        AnimatorFileStoreMapper(const AnimatorFileStoreMapper& mapper) = delete;
        AnimatorFileStoreMapper(AnimatorFileStoreMapper&& mapper) = delete;
        ~AnimatorFileStoreMapper() override;
        AnimatorFileStoreMapper& operator=(const AnimatorFileStoreMapper& mapper) = delete;
        AnimatorFileStoreMapper& operator=(AnimatorFileStoreMapper&& mapper) = delete;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasAnimator(const Animators::AnimatorId& id) override;
        virtual std::optional<Engine::GenericDto> queryAnimator(const Animators::AnimatorId& id) override;
        virtual std::error_code removeAnimator(const Animators::AnimatorId& id) override;
        virtual std::error_code putAnimator(const Animators::AnimatorId& id, const Engine::GenericDto& dto) override;
        virtual std::uint64_t nextSequenceNumber() override;

    protected:
        std::error_code serializeMapperFile();
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::string extractFilename(const Animators::AnimatorId& id, const Engine::FactoryDesc& factory_desc);

        std::error_code serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto);
        Engine::GenericDto deserializeDataTransferObject(const std::string& filename);

    protected:
        bool m_has_connected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Animators::AnimatorId, std::string, Animators::AnimatorId::hash> m_filename_map;
        std::recursive_mutex m_file_map_lock;
        std::uint64_t m_sequence_number;
    };
}

#endif // ANIMATOR_FILE_STORE_MAPPER_H
