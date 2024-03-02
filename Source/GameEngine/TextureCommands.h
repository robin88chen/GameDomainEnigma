/*********************************************************************
 * \file   TextureCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_COMMANDS_H
#define _TEXTURE_COMMANDS_H

#include "FileSystem/IFile.h"
#include "Frameworks/Command.h"
#include "MathLib/Rect.h"
#include "TextureId.h"

namespace Enigma::Engine
{
    class PutTexture : public Frameworks::ICommand
    {
    public:
        PutTexture(const TextureId& id, const std::shared_ptr<Texture>& texture) : m_id(id), m_texture(texture) {}
        const TextureId& id() { return m_id; }
        const std::shared_ptr<Texture>& texture() { return m_texture; }

    private:
        TextureId m_id;
        std::shared_ptr<Texture> m_texture;
    };
    class RemoveTexture : public Frameworks::ICommand
    {
    public:
        RemoveTexture(const TextureId& id) : m_id(id) {}
        const TextureId& id() { return m_id; }

    private:
        TextureId m_id;
    };
    class EnqueueSavingTexture : public Frameworks::ICommand
    {
    public:
        EnqueueSavingTexture(const std::shared_ptr<Texture>& target_texture, const FileSystem::IFilePtr& file) : m_targetTexture(target_texture), m_file(file) {}
        std::shared_ptr<Texture> targetTexture() const { return m_targetTexture; }
        const FileSystem::IFilePtr& file() const { return m_file; }

    private:
        std::shared_ptr<Texture> m_targetTexture;
        std::string m_name;
        FileSystem::IFilePtr m_file;
    };
    class EnqueueRetrievingTextureImage : public Frameworks::ICommand
    {
    public:
        EnqueueRetrievingTextureImage(const std::shared_ptr<Texture>& target_texture, const MathLib::Rect& image_rect) : m_targetTexture(target_texture), m_imageRect(image_rect) {}
        std::shared_ptr<Texture> targetTexture() const { return m_targetTexture; }
        const MathLib::Rect& imageRect() const { return m_imageRect; }

    private:
        std::shared_ptr<Texture> m_targetTexture;
        MathLib::Rect m_imageRect;
    };
    class EnqueueUpdatingTextureImage : public Frameworks::ICommand
    {
    public:
        EnqueueUpdatingTextureImage(const std::shared_ptr<Texture> target_texture, const MathLib::Rect& image_rect, const byte_buffer& image_buff) : m_targetTexture(target_texture), m_imageRect(image_rect), m_imageBuff(image_buff) {}
        std::shared_ptr<Texture> targetTexture() const { return m_targetTexture; }
        const MathLib::Rect& imageRect() const { return m_imageRect; }
        const byte_buffer& imageBuffer() const { return m_imageBuff; }

    private:
        std::shared_ptr<Texture> m_targetTexture;
        MathLib::Rect m_imageRect;
        byte_buffer m_imageBuff;
    };
}

#endif // _TEXTURE_COMMANDS_H
