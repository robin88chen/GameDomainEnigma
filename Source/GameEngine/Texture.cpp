#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IMultiTexture.h"
#include "GraphicKernel/ITexture.h"
#include "Texture.h"
#include "TextureDto.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;

DEFINE_RTTI_OF_BASE(Engine, Texture);

Texture::Texture(const TextureId& id) : m_factoryDesc(TYPE_RTTI.getName())
{
    m_id = id;
    m_lazyStatus.changeStatus(LazyStatus::Status::Ready);
}

Texture::Texture(const TextureId& id, const GenericDto& dto) : m_factoryDesc(TYPE_RTTI.getName())
{
    TextureDto textureDto = TextureDto::fromGenericDto(dto);
    m_id = id;
    m_factoryDesc = textureDto.factoryDesc();
    m_format = textureDto.format();
    m_dimension = textureDto.dimension();
    m_isCubeTexture = textureDto.isCubeTexture();
    m_surfaceCount = textureDto.surfaceCount();
    m_filePaths = textureDto.filePaths();
    m_lazyStatus.changeStatus(LazyStatus::Status::Ghost);
}

Texture::Texture(const TextureId& id, const ITexturePtr& tex) : m_factoryDesc(TYPE_RTTI.getName())
{
    m_id = id;
    m_format = tex->format();
    m_dimension = tex->dimension();
    m_isCubeTexture = tex->isCubeTexture();
    m_surfaceCount = tex->isMultiTexture() ? std::dynamic_pointer_cast<IMultiTexture>(tex)->surfaceCount() : 1;
    m_texture = tex;
    m_lazyStatus.changeStatus(LazyStatus::Status::Ready);
}

Texture::~Texture()
{
    m_texture = nullptr;
}

GenericDto Texture::serializeDto() const
{
    TextureDto textureDto;
    textureDto.factoryDesc() = m_factoryDesc;
    textureDto.format() = m_format;
    textureDto.dimension() = m_dimension;
    textureDto.isCubeTexture() = m_isCubeTexture;
    textureDto.surfaceCount() = m_surfaceCount;
    if (!m_filePaths.empty()) textureDto.filePaths() = m_filePaths;
    return textureDto.toGenericDto();
}

const Enigma::MathLib::Dimension<unsigned>& Texture::dimension() const
{
    return m_dimension;
}

const GraphicFormat& Texture::format() const
{
    return m_format;
}

bool Texture::isCubeTexture() const
{
    return m_isCubeTexture;
}

bool Texture::isMultiTexture() const
{
    return m_surfaceCount > 1;
}

const std::vector<std::string>& Texture::filePaths() const
{
    return m_filePaths;
}

std::vector<std::string>& Texture::filePaths()
{
    return m_filePaths;
}

void Texture::instanceDeviceTexture(const Graphics::ITexturePtr& tex)
{
    assert(m_lazyStatus.isLoading());
    assert(m_isCubeTexture == tex->isCubeTexture());
    assert(isMultiTexture() == tex->isMultiTexture());
    assert(m_dimension == tex->dimension());
    if (isMultiTexture())
    {
        assert(m_surfaceCount == std::dynamic_pointer_cast<IMultiTexture>(tex)->surfaceCount());
    }
    else
    {
        assert(m_surfaceCount == 1);
    }
    m_texture = tex;
    m_lazyStatus.changeStatus(LazyStatus::Status::Ready);
}

