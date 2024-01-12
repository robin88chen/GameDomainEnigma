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
#include "Frameworks/CommandSubscriber.h"
#include <memory>

namespace Enigma::Engine
{
    class Texture;
    class TextureResourceProcessor;

    class TextureFactory
    {
    public:
        TextureFactory();
        ~TextureFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Texture> create(const TextureId& id);
        std::shared_ptr<Texture> constitute(const TextureId& id, const GenericDto& dto, bool is_persisted);

    private:
        void constituteTexture(const Frameworks::ICommandPtr& c);

    private:
        TextureResourceProcessor* m_processor;

        Frameworks::CommandSubscriberPtr m_constituteTexture;
    };
}

#endif // TEXTURE_FACTORY_H
