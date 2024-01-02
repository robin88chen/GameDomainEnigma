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
    class ConstituteTexture : public Frameworks::ICommand
    {
    public:
        ConstituteTexture(const TextureId& id, const GenericDto& dto) : m_id(id), m_dto(dto) {}
        const TextureId& id() { return m_id; }
        const GenericDto& dto() { return m_dto; }

    private:
        TextureId m_id;
        GenericDto m_dto;
    };
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
    class SaveTexture : public Frameworks::IRequestCommand
    {
    public:
        SaveTexture(const std::shared_ptr<Texture>& target_texture, const std::string& name, const FileSystem::IFilePtr& file) : m_targetTexture(target_texture), m_name(name), m_file(file) {}
        std::shared_ptr<Texture> GetTargetTexture() const { return m_targetTexture.lock(); }
        const std::string& GetTextureName() { return m_name; }
        const FileSystem::IFilePtr& GetFile() const { return m_file; }

    private:
        std::weak_ptr<Texture> m_targetTexture;
        std::string m_name;
        FileSystem::IFilePtr m_file;
    };
    class RetrieveTextureImage : public Frameworks::IRequestCommand
    {
    public:
        RetrieveTextureImage(const std::shared_ptr<Texture>& target_texture, const std::string& name, const MathLib::Rect& image_rect) : m_targetTexture(target_texture), m_name(name), m_imageRect(image_rect) {}
        std::shared_ptr<Texture> GetTargetTexture() const { return m_targetTexture.lock(); }
        const std::string& GetTextureName() { return m_name; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }

    private:
        std::weak_ptr<Texture> m_targetTexture;
        std::string m_name;
        MathLib::Rect m_imageRect;
    };
    class UpdateTextureImage : public Frameworks::IRequestCommand
    {
    public:
        UpdateTextureImage(const std::shared_ptr<Texture> target_texture, const std::string& name, const MathLib::Rect& image_rect, const byte_buffer& image_buff) : m_targetTexture(target_texture), m_name(name), m_imageRect(image_rect), m_imageBuff(image_buff) {}
        std::shared_ptr<Texture> GetTargetTexture() const { return m_targetTexture.lock(); }
        const std::string& GetTextureName() { return m_name; }
        const MathLib::Rect& GetImageRect() const { return m_imageRect; }
        const byte_buffer& GetImageBuffer() const { return m_imageBuff; }

    private:
        std::weak_ptr<Texture> m_targetTexture;
        std::string m_name;
        MathLib::Rect m_imageRect;
        byte_buffer m_imageBuff;
    };
}

#endif // _TEXTURE_COMMANDS_H
