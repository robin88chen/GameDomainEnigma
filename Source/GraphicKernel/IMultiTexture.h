/*********************************************************************
 * \file   IMultiTexture.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef MULTI_TEXTURE_INTERFACE_H
#define MULTI_TEXTURE_INTERFACE_H

#include "ITexture.h"
#include "GraphicErrors.h"

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IMultiTexture : public ITexture
    {
    public:
        IMultiTexture(const std::string& name);
        IMultiTexture(const IMultiTexture&) = delete;
        IMultiTexture(IMultiTexture&&) = delete;
        virtual ~IMultiTexture() override;
        IMultiTexture& operator=(const IMultiTexture&) = delete;
        IMultiTexture& operator=(IMultiTexture&&) = delete;

        virtual void multiLoad(const std::vector<byte_buffer>& img_buffs);
        virtual void multiLoad(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids);
        virtual void multiSave(const std::vector<FileSystem::IFilePtr>& files);
        virtual void multiSave(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids);

        virtual void multiCreate(const MathLib::Dimension<unsigned>& dimension, unsigned count, const std::vector<byte_buffer>& buffs);

        virtual bool isMultiTexture() override { return true; }
        virtual unsigned surfaceCount() = 0;

    protected:
        virtual error createFromSystemMemories(const MathLib::Dimension<unsigned>& dimension, unsigned count, const std::vector<byte_buffer>& buffs) = 0;

        virtual error loadTextureImages(const std::vector<byte_buffer>& img_buffs) = 0;
        virtual future_error asyncLoadTextureImages(const std::vector<byte_buffer>& img_buffs);

        virtual error saveTextureImages(const std::vector<FileSystem::IFilePtr>& files) = 0;
        virtual future_error asyncSaveTextureImages(const std::vector<FileSystem::IFilePtr>& files);

        virtual error loadTextureImages(const std::vector<std::string>& filenames,
            const std::vector<std::string>& pathids);
        virtual future_error asyncLoadTextureImages(const std::vector<std::string>& filenames,
            const std::vector<std::string>& pathids);

        virtual error saveTextureImages(const std::vector<std::string>& filenames,
            const std::vector<std::string>& pathids);
        virtual future_error asyncSaveTextureImages(const std::vector<std::string>& filenames,
            const std::vector<std::string>& pathids);

    protected:
        //! not supported funcs
        //virtual er_code CreateAsCubeTexture(unsigned char* img_buffs[6], unsigned int buff_sizes[6])
          //  override final {
            //return ER_NOT_IMPLEMENT;
        //};
        virtual error createFromSystemMemory(const MathLib::Dimension<unsigned>&, const byte_buffer&) override final
        {
            return ErrorCode::notImplement;
        };
        virtual error loadTextureImage(const byte_buffer&) override final
        {
            return ErrorCode::notImplement;
        };
        virtual error loadTextureImage(const std::string& filename, const std::string& pathid) override final
        {
            return ErrorCode::notImplement;
        };
        virtual error UpdateTextureImage(const MathLib::Rect&, const byte_buffer&) override final
        {
            return ErrorCode::notImplement;
        };
        virtual error RetrieveTextureImage(const MathLib::Rect&) override final
        {
            return ErrorCode::notImplement;
        }
        virtual error saveTextureImage(const FileSystem::IFilePtr&) override final
        {
            return ErrorCode::notImplement;
        };
        virtual error saveTextureImage(const std::string& filename, const std::string& pathid) override final
        {
            return ErrorCode::notImplement;
        };
    };
    using IMultiTexturePtr = std::shared_ptr<IMultiTexture>;
    using IMultiTextureWeak = std::weak_ptr<IMultiTexture>;
}


#endif // MULTI_TEXTURE_INTERFACE_H
