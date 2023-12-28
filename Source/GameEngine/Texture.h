/*********************************************************************
 * \file   Texture.h
 * \brief  texture entity
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include "GraphicKernel/ITexture.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "FactoryDesc.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/LazyStatus.h"
#include "TextureId.h"
#include "GenericDto.h"
#include "Frameworks/EventSubscriber.h"
#include <string>

namespace Enigma::Engine
{
    class Texture
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        Texture(const TextureId& id);
        Texture(const TextureId& id, const GenericDto& dto);
        Texture(const TextureId& id, const Graphics::ITexturePtr& tex);
        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        virtual ~Texture();
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        const TextureId& id() const { return m_id; }

        const Frameworks::LazyStatus& lazyStatus() const { return m_lazyStatus; }
        Frameworks::LazyStatus& lazyStatus() { return m_lazyStatus; }

        GenericDto serializeDto() const;

        const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        FactoryDesc& factoryDesc() { return m_factoryDesc; }

        const Graphics::ITexturePtr& getDeviceTexture() { return m_texture; }
        void instanceDeviceTexture(const Graphics::ITexturePtr& tex);

        const Graphics::GraphicFormat& format() const;
        const MathLib::Dimension<unsigned>& dimension() const;
        bool isCubeTexture() const;
        bool isMultiTexture() const;
        const std::vector<std::string>& filePaths() const;
        std::vector<std::string>& filePaths();

    protected:
        TextureId m_id;
        Frameworks::LazyStatus m_lazyStatus;
        Graphics::GraphicFormat m_format;
        MathLib::Dimension<unsigned> m_dimension;
        bool m_isCubeTexture;
        unsigned m_surfaceCount;
        std::vector<std::string> m_filePaths;
        FactoryDesc m_factoryDesc;
        Graphics::ITexturePtr m_texture;
    };
    using TexturePtr = std::shared_ptr<Texture>;
}

#endif // TEXTURE_H
