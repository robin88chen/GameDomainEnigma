/*********************************************************************
 * \file   TextureLoader.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "TextureLoadingPolicies.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::Engine
{
    class TextureManager;
	class TextureLoader
	{
	public:
        TextureLoader(TextureManager* host);
        TextureLoader(const TextureLoader&) = delete;
        TextureLoader(TextureLoader&&) = delete;
        ~TextureLoader();
        TextureLoader& operator=(TextureLoader&) = delete;
        TextureLoader& operator=(TextureLoader&&) = delete;

        void LoadTexture(const TexturePolicy& policy);

    private:
        void OnTextureCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e);

	private:
        TextureManager* m_hostManager;
        TexturePolicy m_policy;

		Enigma::Frameworks::EventSubscriberPtr m_onTextureCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureImageLoaded;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureLoadImageFailed;
    };
}

#endif // TEXTURE_LOADER_H
