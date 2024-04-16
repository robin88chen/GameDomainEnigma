/*********************************************************************
 * \file   EffectMaterialSourceFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_MATERIAL_SOURCE_FILE_STORE_MAPPER_H
#define EFFECT_MATERIAL_SOURCE_FILE_STORE_MAPPER_H

#include "GameEngine/EffectMaterialSourceStoreMapper.h"
#include "GameEngine/EffectCompilingProfile.h"
#include "Gateways/EffectProfileJsonGateway.h"

namespace Enigma::FileStorage
{
    class EffectMaterialSourceFileStoreMapper : public Engine::EffectMaterialSourceStoreMapper
    {
    public:
        EffectMaterialSourceFileStoreMapper(const std::string& mapper_filename);
        ~EffectMaterialSourceFileStoreMapper() override;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasEffectMaterial(const Engine::EffectMaterialId& id) override;
        virtual std::optional<Engine::EffectCompilingProfile> queryEffectMaterial(const Engine::EffectMaterialId& id) override;

    protected:
        void deserializeMapperFile(const std::string& mapper_file_content);
        std::optional<Engine::EffectCompilingProfile> deserializeEffectCompilingProfile(const std::string& filename);

    protected:
        bool m_has_connected;
        Gateways::EffectProfileJsonGateway m_gateway;
        std::string m_mapper_filename;
        std::unordered_map<Engine::EffectMaterialId, std::string, Engine::EffectMaterialId::hash> m_filename_map;
        mutable std::recursive_mutex m_fileMapLock;
    };
}

#endif // EFFECT_MATERIAL_SOURCE_FILE_STORE_MAPPER_H
