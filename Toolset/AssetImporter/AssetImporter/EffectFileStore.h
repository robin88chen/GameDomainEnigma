/*********************************************************************
 * \file   EffectFileStore.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef EFFECT_FILE_STORE_H
#define EFFECT_FILE_STORE_H

#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"

namespace AssetImporter
{
    class EffectFileStore : public Enigma::FileStorage::EffectMaterialSourceFileStoreMapper
    {
    public:
        EffectFileStore(const std::string& mapper_filename) : Enigma::FileStorage::EffectMaterialSourceFileStoreMapper(mapper_filename) {}

        const std::unordered_map<Enigma::Engine::EffectMaterialId, std::string, Enigma::Engine::EffectMaterialId::hash>& filenameMap() { return m_filename_map; }

        void appendEffectMaterial(const Enigma::Engine::EffectMaterialId& id, const std::string& filename);
        void serializeMapperFile();
    };
}


#endif // EFFECT_FILE_STORE_H
