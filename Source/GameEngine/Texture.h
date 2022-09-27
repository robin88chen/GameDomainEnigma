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
#include <string>

namespace Enigma::Engine
{
    class Texture
    {
    public:
        Texture(const std::string& name, const Graphics::ITexturePtr& tex);
        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        virtual ~Texture();
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        const std::string& GetName() const { return m_name; }

        const Graphics::ITexturePtr& GetDeviceTexture() { return m_texture; }

        virtual const Graphics::GraphicFormat& GetFormat();
        virtual const MathLib::Dimension& GetDimension();
        virtual bool IsCubeTexture();
        virtual bool IsMultiTexture() { return false; }

    protected:
        std::string m_name;
        Graphics::ITexturePtr m_texture;
    };
    using TexturePtr = std::shared_ptr<Texture>;
}

#endif // TEXTURE_H
