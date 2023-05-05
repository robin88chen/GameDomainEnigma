#include "MultiTextureDx11.h"
#include "GraphicAPIDx11.h"
#include "MultiBackSurfaceDx11.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicEvents.h"
#include "FileSystem/IFile.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

extern DXGI_FORMAT ConvertGraphicFormatToDXGI(const Enigma::Graphics::GraphicFormat& format);
extern unsigned int ConvertDXGIFormatToGraphicFormat(DXGI_FORMAT fmt);

MultiTextureDx11::MultiTextureDx11(const std::string& name) : IMultiTexture(name)
{
    m_d3dTextureResources = nullptr;
    m_resourceViewCount = 0;
}

MultiTextureDx11::~MultiTextureDx11()
{
    if ((m_d3dTextureResources) && (m_resourceViewCount > 0))
    {
        for (unsigned int i = 0; i < m_resourceViewCount; i++)
        {
            SAFE_RELEASE(m_d3dTextureResources[i]);
        }
    }
    SAFE_DELETE_ARRAY(m_d3dTextureResources);
}

error MultiTextureDx11::LoadTextureImages(const std::vector<byte_buffer>& img_buffs)
{
    assert(!img_buffs.empty());
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    if ((m_d3dTextureResources) && (m_resourceViewCount > 0))
    {
        for (unsigned int i = 0; i < m_resourceViewCount; i++)
        {
            SAFE_RELEASE(m_d3dTextureResources[i]);
        }
    }
    SAFE_DELETE_ARRAY(m_d3dTextureResources);
    m_resourceViewCount = static_cast<unsigned int>(img_buffs.size());

    m_d3dTextureResources = new ID3D11ShaderResourceView * [m_resourceViewCount];
    memset(m_d3dTextureResources, 0, m_resourceViewCount * sizeof(ID3D11ShaderResourceView*));

    for (unsigned int i = 0; i < m_resourceViewCount; i++)
    {
        DirectX::TexMetadata metaData;
        DirectX::ScratchImage scratchImage;
        if ((img_buffs[i][0] == 'D') && (img_buffs[i][1] == 'D')
            && (img_buffs[i][2] == 'S') && (img_buffs[i][3] == ' '))
        {
            HRESULT hr = DirectX::LoadFromDDSMemory(&img_buffs[i][0], img_buffs[i].size(), 0, &metaData, scratchImage);
            if (FAILED(hr))
            {
                return ErrorCode::dxLoadTexture;
            }
        }
        else
        {
            HRESULT hr = DirectX::LoadFromWICMemory(&img_buffs[i][0], img_buffs[i].size(),
                DirectX::WIC_FLAGS_FORCE_RGB, &metaData, scratchImage);
            if (FAILED(hr))
            {
                return ErrorCode::dxLoadTexture;
            }
        }

        error er = CreateFromScratchImage(i, scratchImage);
        if (er) return er;
    }

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::MultiTextureResourceImagesLoaded>(m_name));
    return ErrorCode::ok;
}

error MultiTextureDx11::SaveTextureImages(const std::vector<FileSystem::IFilePtr>& files)
{
    assert(!files.empty());
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;
    ID3D11DeviceContext* deviceContext = api_dx11->GetD3DDeviceContext();
    if (FATAL_LOG_EXPR(!deviceContext)) return ErrorCode::d3dDeviceNullPointer;
    if ((!m_d3dTextureResources) || (m_resourceViewCount == 0)) return ErrorCode::nullDxTexture;

    size_t count = files.size();
    if (count > m_resourceViewCount) count = m_resourceViewCount;
    for (size_t i = 0; i < count; i++)
    {
        if (!m_d3dTextureResources[i]) return ErrorCode::nullDxTexture;
        if (!files[i]) return ErrorCode::fileIO;

        ID3D11Resource* d3dResource;
        m_d3dTextureResources[i]->GetResource(&d3dResource);
        if (!d3dResource) return ErrorCode::nullDxTexture;

        DirectX::ScratchImage resultImage;
        HRESULT hr = DirectX::CaptureTexture(device, deviceContext, d3dResource, resultImage);
        if (FATAL_LOG_EXPR(FAILED(hr))) return ErrorCode::dxSaveTexture;

        DirectX::Blob blob;
        DirectX::SaveToDDSMemory(resultImage.GetImages(), resultImage.GetImageCount(), resultImage.GetMetadata(), 0, blob);
        byte_buffer write_buff = make_data_buffer(static_cast<unsigned char*>(blob.GetBufferPointer()), blob.GetBufferSize());
        size_t write_bytes = files[i]->Write(0, write_buff);
        if (write_bytes != write_buff.size()) return ErrorCode::saveTextureFile;
    }

    std::vector<std::string> filenames;
    for (size_t i = 0; i < count; i++)
    {
        filenames.emplace_back(files[i]->GetFullPath());
    }
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::MultiTextureResourceImagesSaved>(m_name, filenames));
    return ErrorCode::ok;
}

error MultiTextureDx11::UseAsBackSurface(const Graphics::IBackSurfacePtr& back_surf)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    if (FATAL_LOG_EXPR(!back_surf)) return ErrorCode::nullBackSurface;
    MultiBackSurfaceDx11* bbDx11 = dynamic_cast<MultiBackSurfaceDx11*>(back_surf.get());
    assert(bbDx11);

    if ((m_d3dTextureResources) && (m_resourceViewCount > 0))
    {
        for (unsigned int i = 0; i < m_resourceViewCount; i++)
        {
            SAFE_RELEASE(m_d3dTextureResources[i]);
        }
    }
    SAFE_DELETE_ARRAY(m_d3dTextureResources);
    m_resourceViewCount = bbDx11->GetSurfaceCount();

    m_d3dTextureResources = new ID3D11ShaderResourceView * [m_resourceViewCount];
    memset(m_d3dTextureResources, 0, m_resourceViewCount * sizeof(ID3D11ShaderResourceView*));

    m_dimension = bbDx11->GetDimension();
    m_format = back_surf->GetFormat();

    for (unsigned int i = 0; i < m_resourceViewCount; i++)
    {
        // backbuffer surface不能直接拿來用，要建立一個shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
        ID3D11ShaderResourceView* d3dResource = nullptr;

        SRDesc.Format = ConvertGraphicFormatToDXGI(bbDx11->GetSliceFormat(i));
        SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRDesc.Texture2D.MostDetailedMip = 0;
        SRDesc.Texture2D.MipLevels = 1;
        HRESULT hr = device->CreateShaderResourceView(bbDx11->GetD3DSurface(i), &SRDesc, &d3dResource);
        if (FATAL_LOG_EXPR(S_OK != hr)) return ErrorCode::dxCreateShaderResource;
        m_d3dTextureResources[i] = d3dResource;
    }
    Frameworks::EventPublisher::Post(std::make_shared<Graphics::MultiTextureResourcesAsBackSurfaceUsed>(
        m_name, back_surf->GetName()));
    return ErrorCode::ok;
}

ID3D11ShaderResourceView* MultiTextureDx11::GetD3DResourceView(unsigned idx) const
{
    if ((m_d3dTextureResources) && (idx < m_resourceViewCount)) return m_d3dTextureResources[idx];
    return nullptr;
}

ID3D11ShaderResourceView** MultiTextureDx11::GetD3DResourceViewArray() const
{
    return m_d3dTextureResources;
}

unsigned int MultiTextureDx11::GetResourceViewCount() const
{
    return m_resourceViewCount;
}

error MultiTextureDx11::CreateFromScratchImage(unsigned index, DirectX::ScratchImage& scratchImage)
{
    byte_buffer img_buff;
    img_buff.resize(scratchImage.GetPixelsSize());
    memcpy(&img_buff[0], scratchImage.GetPixels(), scratchImage.GetPixelsSize());
    return CreateOneFromSystemMemory(index,
        MathLib::Dimension{ static_cast<unsigned int>(scratchImage.GetMetadata().width), static_cast<unsigned int>(scratchImage.GetMetadata().height) },
        img_buff);
}

error MultiTextureDx11::CreateOneFromSystemMemory(unsigned index, const MathLib::Dimension& dimension, const byte_buffer& buff)
{
    GraphicAPIDx11* api_dx11 = dynamic_cast<GraphicAPIDx11*>(Graphics::IGraphicAPI::Instance());
    assert(api_dx11);
    ID3D11Device* device = api_dx11->GetD3DDevice();
    if (FATAL_LOG_EXPR(!device)) return ErrorCode::d3dDeviceNullPointer;

    D3D11_TEXTURE2D_DESC tex_desc;
    ZeroMemory(&tex_desc, sizeof(tex_desc));
    tex_desc.Width = (unsigned int)dimension.m_width;
    tex_desc.Height = (unsigned int)dimension.m_height;
    tex_desc.ArraySize = 1;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.MipLevels = 1;

    ID3D11Texture2D* texture2D = 0;

    if (!buff.empty())
    {
        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
        data.pSysMem = &buff[0];
        data.SysMemPitch = 4 * tex_desc.Width;

        HRESULT hr = device->CreateTexture2D(&tex_desc, &data, &texture2D);
        if (FAILED(hr)) return ErrorCode::deviceCreateTexture;
    }
    else
    {
        HRESULT hr = device->CreateTexture2D(&tex_desc, 0, &texture2D);
        if (FAILED(hr)) return ErrorCode::deviceCreateTexture;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
    ID3D11ShaderResourceView* d3dResource = 0;

    SRDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRDesc.Texture2D.MostDetailedMip = 0;
    SRDesc.Texture2D.MipLevels = 1;
    HRESULT hr = device->CreateShaderResourceView(texture2D, &SRDesc, &d3dResource);
    if (FATAL_LOG_EXPR(S_OK != hr)) return ErrorCode::deviceCreateTexture;
    m_d3dTextureResources[index] = d3dResource;
    texture2D->Release();

    m_dimension = dimension;
    m_format = ConvertDXGIFormatToGraphicFormat(SRDesc.Format);

    return ErrorCode::ok;
}
