/*********************************************************************
 * \file   TextureFileStore.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef TEXTURE_FILES_TORE_H
#define TEXTURE_FILES_TORE_H

#include "FileStorage/TextureFileStoreMapper.h"

namespace AssetImporter
{
    class TextureFileStore : public Enigma::FileStorage::TextureFileStoreMapper
    {
    public:
        TextureFileStore(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::TextureFileStoreMapper(mapper_filename, gateway) {}

        const std::unordered_map<Enigma::Engine::TextureId, std::string, Enigma::Engine::TextureId::hash>& filenameMap() { return m_filename_map; }
    };
}

#endif // TEXTURE_FILES_TORE_H
