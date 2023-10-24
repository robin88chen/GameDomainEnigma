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
#include <string>

namespace Enigma::Engine
{
    class Texture
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        Texture(const std::string& name, const Graphics::ITexturePtr& tex);
        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        virtual ~Texture();
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        const FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }

        const std::string& getName() const { return m_name; }

        const Graphics::ITexturePtr& GetDeviceTexture() { return m_texture; }

        virtual const Graphics::GraphicFormat& GetFormat();
        virtual const MathLib::Dimension<unsigned>& GetDimension();
        virtual bool IsCubeTexture();
        virtual bool IsMultiTexture() { return m_texture ? m_texture->IsMultiTexture() : false; }

    protected:
        FactoryDesc m_factoryDesc;
        std::string m_name;
        Graphics::ITexturePtr m_texture;
    };
    using TexturePtr = std::shared_ptr<Texture>;
}

#endif // TEXTURE_H
