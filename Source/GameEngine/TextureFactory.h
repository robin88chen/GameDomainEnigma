/*********************************************************************
 * \file   TextureFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_FACTORY_H
#define TEXTURE_FACTORY_H

#include "TextureId.h"
#include "GenericDto.h"
#include <memory>

namespace Enigma::Engine
{
    class Texture;

    class TextureFactory
    {
    public:
        TextureFactory();
        ~TextureFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Texture> create(const TextureId& id);
        std::shared_ptr<Texture> constitute(const TextureId& id, const GenericDto& dto);
    };
}

#endif // TEXTURE_FACTORY_H
