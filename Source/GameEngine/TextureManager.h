/*********************************************************************
 * \file   TextureManager.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"

namespace Enigma::Engine
{
	class TextureManager : public Frameworks::ISystemService
	{
        DECLARE_EN_RTTI;
    public:
        TextureManager(Frameworks::ServiceManager* srv_manager);
        TextureManager(const TextureManager&) = delete;
        TextureManager(TextureManager&&) = delete;
        ~TextureManager();
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;
	};
}

#endif // TEXTURE_MANAGER_H
