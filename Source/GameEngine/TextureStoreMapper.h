/*********************************************************************
 * \file   TextureStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_STORE_MAPPER_H
#define TEXTURE_STORE_MAPPER_H

#include "GenericDto.h"
#include <memory>
#include <system_error>

namespace Enigma::Engine
{
    class Texture;
    class TextureId;

    class TextureStoreMapper
    {
    public:
        virtual ~TextureStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasTexture(const TextureId& id) = 0;
        virtual std::optional<GenericDto> queryTexture(const TextureId& id) = 0;
        virtual std::error_code removeTexture(const TextureId& id) = 0;
        virtual std::error_code putTexture(const TextureId& id, const GenericDto& dto) = 0;
    };
}

#endif // TEXTURE_STORE_MAPPER_H
