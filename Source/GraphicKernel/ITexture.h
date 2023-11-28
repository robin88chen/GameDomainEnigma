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
#include "RenderTextureUsage.h"
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
        ITexture(const std::string& name);
        ITexture(const ITexture&) = delete;
        ITexture(ITexture&&) = delete;
        virtual ~ITexture();
        ITexture& operator=(const ITexture&) = delete;
        ITexture& operator=(ITexture&&) = delete;

        const std::string& getName() { return m_name; }

        virtual void Load(const byte_buffer& img_buff);
        virtual void Load(const std::string& filename, const std::string& pathid);
        virtual void Save(const FileSystem::IFilePtr& file);
        virtual void Save(const std::string& filename, const std::string& pathid);

        virtual void Create(const MathLib::Dimension<unsigned>& dimension, const byte_buffer& buff);

        virtual void Retrieve(const MathLib::Rect& rcSrc);
        virtual void update(const MathLib::Rect& rcDest, const byte_buffer& img_buff);

        virtual void AsBackSurface(const IBackSurfacePtr& back_surf, const std::vector<RenderTextureUsage>&);

        virtual const GraphicFormat& GetFormat() { return m_format; };
        virtual const MathLib::Dimension<unsigned>& GetDimension() { return m_dimension; };
        const byte_buffer& GetRetrievedBuffer() { return m_retrievedBuff; }
        virtual bool IsCubeTexture() { return m_isCubeTexture; }

        virtual bool IsMultiTexture() { return false; }

    protected:
        virtual error LoadTextureImage(const byte_buffer& img_buff) = 0;
        virtual error LoadTextureImage(const std::string& filename, const std::string& pathid);
        virtual error CreateFromSystemMemory(const MathLib::Dimension<unsigned>& dimension, const byte_buffer& buff) = 0;
        virtual error SaveTextureImage(const FileSystem::IFilePtr& file) = 0;
        virtual error RetrieveTextureImage(const MathLib::Rect& rcSrc) = 0;
        virtual error UpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff) = 0;
        virtual error SaveTextureImage(const std::string& filename, const std::string& pathid);
        virtual error UseAsBackSurface(const IBackSurfacePtr& back_surf, const std::vector<RenderTextureUsage>& usages) = 0;

    protected:
        std::string m_name;
        bool m_isCubeTexture;
        MathLib::Dimension<unsigned> m_dimension;

        GraphicFormat m_format;

        byte_buffer m_retrievedBuff;
    };
    using ITexturePtr = std::shared_ptr<ITexture>;
    using ITextureWeak = std::weak_ptr<ITexture>;
}

#endif // TEXTURE_INTERFACE_H
