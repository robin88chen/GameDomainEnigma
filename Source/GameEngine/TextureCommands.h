/*********************************************************************
 * \file   TextureCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_COMMANDS_H
#define _TEXTURE_COMMANDS_H

#include "TexturePolicies.h"
#include "Frameworks/Command.h"
#include "MathLib/Rect.h"

namespace Enigma::Engine
{
    class LoadTexture : public Frameworks::IRequestCommand
    {
    public:
        LoadTexture(const TexturePolicy& policy) : m_policy(policy) {}
        const TexturePolicy& GetPolicy() { return m_policy; }

    private:
        TexturePolicy m_policy;
    };
    class CreateTexture : public Frameworks::IRequestCommand
    {
    public:
        CreateTexture(const TexturePolicy& policy) : m_policy(policy) {}
        const TexturePolicy& GetPolicy() { return m_policy; }

    private:
        TexturePolicy m_policy;
    };
    class RetrieveTextureImage : public Frameworks::IRequestCommand
    {
    public:
        RetrieveTextureImage(const std::string& name, const MathLib::Rect& image_rect) : m_name(name), m_imageRect(image_rect) {}
        const std::string& GetTextureName() { return m_name; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }

    private:
        std::string m_name;
        MathLib::Rect m_imageRect;
    };
    class UpdateTextureImage : public Frameworks::IRequestCommand
    {
    public:
        UpdateTextureImage(const std::string& name, const MathLib::Rect& image_rect, const byte_buffer& image_buff) : m_name(name), m_imageRect(image_rect), m_imageBuff(image_buff) {}
        const std::string& GetTextureName() { return m_name; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
        const byte_buffer& GetImageBuffer() const { return m_imageBuff; }

    private:
        std::string m_name;
        MathLib::Rect m_imageRect;
        byte_buffer m_imageBuff;
    };
}

#endif // _TEXTURE_COMMANDS_H
