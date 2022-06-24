/*********************************************************************
 * \file   ITexture.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef TEXTURE_INTERFACE_H
#define TEXTURE_INTERFACE_H

#include "GraphicAPITypes.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <memory>
#include <system_error>


namespace Enigma::MathLib
{
    class Rect;
}
namespace Enigma::FileSystem
{
    class IFile;
    using IFilePtr = std::shared_ptr<IFile>;
}

namespace Enigma::Graphics
{
    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;

    using error = std::error_code;

    class ITexture : public std::enable_shared_from_this<ITexture>
    {
    public:
        ITexture();
        ITexture(const ITexture&) = delete;
        virtual ~ITexture();
        ITexture& operator=(const ITexture&) = delete;

        virtual error CreateFromSystemMemory(const MathLib::Dimension& dimension, const byte_buffer& buff) = 0;
        virtual future_error AsyncCreateFromSystemMemory(const MathLib::Dimension& dimension, const byte_buffer& buff);
        virtual error LoadTextureImage(const byte_buffer& img_buff) = 0;
        virtual future_error AsyncLoadTextureImage(const byte_buffer& img_buff);
        virtual error SaveTextureImage(const FileSystem::IFilePtr& file) = 0;
        virtual future_error AsyncSaveTextureImage(const FileSystem::IFilePtr& file);

        virtual error RetrieveTextureImage(const MathLib::Rect& rcSrc, byte_buffer* target_buff) = 0;
        virtual future_error AsyncRetrieveTextureImage(const MathLib::Rect& rcSrc, byte_buffer* target_buff);

        virtual error LoadTextureImage(const std::string& filename, const std::string& pathid);
        virtual future_error AsyncLoadTextureImage(const std::string& filename, const std::string& pathid);

        virtual error UpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff) = 0;
        virtual future_error AsyncUpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff);
        virtual error SaveTextureImage(const std::string& filename, const std::string& pathid);
        virtual future_error AsyncSaveTextureImage(const std::string& filename, const std::string& pathid);

        virtual error UseAsBackSurface(const IBackSurfacePtr& back_surf) = 0;
        virtual future_error AsyncUseAsBackSurface(const IBackSurfacePtr& back_surf);

        virtual const GraphicFormat& GetFormat() { return m_format; };
        virtual const MathLib::Dimension& GetDimension() { return m_dimension; };
        virtual bool IsCubeTexture() { return m_isCubeTexture; }

        virtual bool IsMultiTexture() { return false; }

    protected:
        bool m_isCubeTexture;
        MathLib::Dimension m_dimension;

        GraphicFormat m_format;
    };
    using ITexturePtr = std::shared_ptr<ITexture>;

}

#endif // TEXTURE_INTERFACE_H
