#include "Texture.h"
#include "TextureDto.h"
#include "GraphicKernel/ITexture.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"
#include <cassert>

#include "EngineErrors.h"
#include "TextureEvents.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IMultiTexture.h"
#include "Platforms/PlatformLayer.h"

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
    m_format = textureDto.format();
    m_dimension = textureDto.dimension();
    m_isCubeTexture = textureDto.isCubeTexture();
    m_surfaceCount = textureDto.surfaceCount();
    m_filePaths = textureDto.filePaths();
    m_lazyStatus.changeStatus(LazyStatus::Status::Ghost);
}

Texture::Texture(const std::string& name, const ITexturePtr& tex) : m_factoryDesc(TYPE_RTTI.getName())
{
    m_texture = tex;
}

Texture::~Texture()
{
    m_texture = nullptr;
}

GenericDto Texture::serializeDto() const
{
    TextureDto textureDto;
    textureDto.format() = m_format;
    textureDto.dimension() = m_dimension;
    textureDto.isCubeTexture() = m_isCubeTexture;
    textureDto.surfaceCount() = m_surfaceCount;
    if (!m_filePaths.empty()) textureDto.filePaths() = m_filePaths;
    return textureDto.toGenericDto();
}

const Enigma::MathLib::Dimension<unsigned>& Texture::dimension()
{
    return m_dimension;
}

const GraphicFormat& Texture::format()
{
    return m_format;
}

bool Texture::isCubeTexture()
{
    return m_isCubeTexture;
}

bool Texture::isMultiTexture()
{
    return m_surfaceCount > 1;
}

void Texture::instanceLazyContent()
{
    assert(m_lazyStatus.isGhost());
    if (isMultiTexture())
    {
        CommandBus::post(std::make_shared<CreateMultiTexture>(m_id.name()));
    }
    else
    {
        CommandBus::post(std::make_shared<CreateTexture>(m_id.name()));
    }
    m_lazyStatus.changeStatus(LazyStatus::Status::Loading);
}

void Texture::loadResourceTextures()
{
    assert(m_lazyStatus.isLoading());
    assert(m_texture);
    if (isMultiTexture())
    {
        std::dynamic_pointer_cast<IMultiTexture>(m_texture)->multiLoad(m_filePaths, {});
    }
    else
    {
        m_texture->load(m_filePaths[0], "");
    }
}

void Texture::createEmptyResourceTextures()
{
    assert(m_lazyStatus.isLoading());
    if (isMultiTexture())
    {
        std::vector<byte_buffer> buffers;
        buffers.resize(m_surfaceCount);
        std::dynamic_pointer_cast<IMultiTexture>(m_texture)->multiCreate(m_dimension, m_surfaceCount, buffers);
    }
    else
    {
        m_texture->create(m_dimension, byte_buffer{});
    }
}

void Texture::onDeviceTextureCreated(const IEventPtr& e)
{
    assert(m_lazyStatus.isLoading());
    if (const auto ev = std::dynamic_pointer_cast<DeviceTextureCreated>(e))
    {
        if (ev->textureName() != m_id.name()) return;
        const auto texture = IGraphicAPI::instance()->TryFindGraphicAsset<ITexturePtr>(m_id.name());
        if (!texture)
        {
            Platforms::Debug::Printf("can't get texture asset %s", m_id.name().c_str());
            EventPublisher::post(std::make_shared<LoadTextureFailed>(m_id, ErrorCode::findStashedAssetFail));
            return;
        }
        m_texture = texture.value();
    }
    else if (const auto ev = std::dynamic_pointer_cast<DeviceMultiTextureCreated>(e))
    {
        if (ev->textureName() != m_id.name()) return;
        const auto texture = IGraphicAPI::instance()->TryFindGraphicAsset<ITexturePtr>(m_id.name());
        if (!texture)
        {
            Platforms::Debug::Printf("can't get texture asset %s", m_id.name().c_str());
            EventPublisher::post(std::make_shared<LoadTextureFailed>(m_id, ErrorCode::findStashedAssetFail));
            return;
        }
        m_texture = texture.value();
    }
    else return;
    if (!m_filePaths.empty())
    {
        loadResourceTextures();
    }
    else
    {
        createEmptyResourceTextures();
    }
}
