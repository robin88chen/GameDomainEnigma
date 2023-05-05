#include "GraphicAPIDx11.h"
#include "SwapChainDx11.h"
#include "AdapterDx11.h"
#include "DeviceCreatorDx11.h"
#include "BackSurfaceDx11.h"
#include "MultiBackSurfaceDx11.h"
#include "DepthStencilSurfaceDx11.h"
#include "TextureDx11.h"
#include "MultiTextureDx11.h"
#include "VertexBufferDx11.h"
#include "IndexBufferDx11.h"
#include "VertexShaderDx11.h"
#include "PixelShaderDx11.h"
#include "ShaderProgramDx11.h"
#include "VertexDeclarationDx11.h"
#include "DeviceSamplerStateDx11.h"
#include "DeviceAlphaBlendStateDx11.h"
#include "DeviceRasterizerStateDx11.h"
#include "DeviceDepthStencilStateDx11.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

static D3D11_PRIMITIVE_TOPOLOGY ConvertTopologyD3D11(Enigma::Graphics::PrimitiveTopology pt)
{
    return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(pt);
}

GraphicAPIDx11::GraphicAPIDx11(AsyncType async) : IGraphicAPI(async)
{
    m_apiVersion = IGraphicAPI::APIVersion::API_Dx11;
    m_wnd = nullptr;
    m_creator = nullptr;
    m_swapChain = nullptr;
    m_adapter = nullptr;

    m_d3dDevice = nullptr;
    m_d3dDeviceContext = nullptr;
}

GraphicAPIDx11::~GraphicAPIDx11()
{
    SAFE_DELETE(m_creator);
    SAFE_DELETE(m_swapChain);
    SAFE_DELETE(m_adapter);
}

error GraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    Platforms::Debug::Printf("create dx11 device in thread %d\n", std::this_thread::get_id());
    m_deviceRequiredBits = rqb;
    m_wnd = hwnd;
    m_adapter = menew AdapterDx11();
    m_swapChain = menew SwapChainDx11();
    m_creator = menew DeviceCreatorDx11();
    m_creator->Initialize(static_cast<HWND>(m_wnd), rqb);
    m_creator->BuildDeviceList(m_adapter);

    SAFE_RELEASE(m_d3dDevice);

    error er = m_creator->CreateWindowedDevice(m_adapter, m_swapChain, &m_d3dDevice, &m_d3dDeviceContext);
    if (er) return er;
    AddDebugInfoFilter();
    return er;
}

error GraphicAPIDx11::CleanupDevice()
{
    Platforms::Debug::Printf("cleanup device in thread %d\n", std::this_thread::get_id());
    m_stash->Clear();
    CleanupDeviceObjects();

    SAFE_RELEASE(m_d3dDevice);
    SAFE_RELEASE(m_d3dDeviceContext);
    return Graphics::ErrorCode::ok;
}

error GraphicAPIDx11::BeginDrawingScene()
{
    //Platforms::Debug::Printf("begin scene in thread %d\n", std::this_thread::get_id());
    return ErrorCode::ok;
}

error GraphicAPIDx11::EndDrawingScene()
{
    //Platforms::Debug::Printf("end scene in thread %d\n", std::this_thread::get_id());
    return ErrorCode::ok;
}

error GraphicAPIDx11::DrawPrimitive(unsigned vertexCount, unsigned vertexOffset)
{
    if (FATAL_LOG_EXPR(m_d3dDeviceContext)) return ErrorCode::d3dDeviceNullPointer;
    m_d3dDeviceContext->Draw(vertexCount, vertexOffset);
    return ErrorCode::ok;
}

error GraphicAPIDx11::DrawIndexedPrimitive(unsigned indexCount, unsigned vertexCount, unsigned indexOffset,
    int baseVertexOffset)
{
    if (FATAL_LOG_EXPR(!m_d3dDeviceContext)) return ErrorCode::d3dDeviceNullPointer;
    m_d3dDeviceContext->DrawIndexed(indexCount, indexOffset, baseVertexOffset);
    return ErrorCode::ok;
}

error GraphicAPIDx11::FlipBackSurface()
{
    assert(m_swapChain);
    //DebugPrintf("flip in thread %d\n", std::this_thread::get_id());
    error er = m_swapChain->Present();
    //! 這是要處理同一個 Resource View 被綁在 Render Target & PS 上的 Warning,
    //! 設定後會清掉 Warning, 也會增加些效能,
    //! 但目前的效能差異還看不出來, 同時, 狀態紀錄錯誤的狀況也還看不出來
    //! 所以先這樣做了, 日後有問題再處理
    //! 有問題!!! 狀態被清光，連續畫同一個 vertex buffer or shader時, 就不會重設了
    //! 解決方式 : 從 D3D Info Queue 將警告關掉
    //m_d3dDeviceContext->ClearState();
    return er;
}

error GraphicAPIDx11::CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    Platforms::Debug::Printf("create primary back surface in thread %d\n", std::this_thread::get_id());
    if (FATAL_LOG_EXPR(!m_d3dDevice)) return ErrorCode::d3dDeviceNullPointer;

    ID3D11Texture2D* d3dbackTex = GetPrimaryD3DBackbuffer();
    if (!d3dbackTex)
    {
        return ErrorCode::createBackSurfaceFail;
    }
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceDx11{back_name, m_d3dDevice, d3dbackTex, true} };
    SetPrimaryBackSurfaceFormat(back_surface->GetFormat());
    m_stash->Add(back_name, back_surface);

    if ((m_deviceRequiredBits.m_usesDepthBuffer) && (!depth_name.empty()))
    {
        auto dimension = back_surface->GetDimension();
        Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
            menew DepthStencilSurfaceDx11{ depth_name, m_d3dDevice, dimension, Graphics::GraphicFormat::FMT_D24S8 } };
        SetDepthSurfaceFormat(depth_surface->GetFormat());
        m_stash->Add(depth_name, depth_surface);
    }
    if (d3dbackTex) d3dbackTex->Release();

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::PrimarySurfaceCreated>(back_name, depth_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    const Graphics::GraphicFormat& fmt)
{
    Platforms::Debug::Printf("create back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceDx11{ back_name, GetD3DDevice(), dimension, fmt } };
    m_stash->Add(back_name, back_surface);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::BackSurfaceCreated>(back_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    unsigned buff_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    Platforms::Debug::Printf("create back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew MultiBackSurfaceDx11{ back_name, GetD3DDevice(), dimension, buff_count, fmts } };
    m_stash->Add(back_name, back_surface);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::MultiBackSurfaceCreated>(back_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    Platforms::Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceDx11{ depth_name, GetD3DDevice(), dimension, fmt } };
    m_stash->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DepthSurfaceCreated>(depth_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::ShareDepthStencilSurface(const std::string& depth_name, 
    const Graphics::IDepthStencilSurfacePtr& from_depth)
{
    Platforms::Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    auto depth_dx11 = std::dynamic_pointer_cast<DepthStencilSurfaceDx11, Graphics::IDepthStencilSurface>(from_depth);
    assert(depth_dx11);
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceDx11{ depth_name, depth_dx11 } };
    m_stash->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DepthSurfaceShared>(depth_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::ClearSurface(const Graphics::IBackSurfacePtr& back_surface, 
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, 
    float depth_value, unsigned int stencil_value)
{
    error er = ErrorCode::ok;
    if (back_surface)
    {
        if (!back_surface->IsMultiSurface())
        {
            er = ClearSingleBackSurface(back_surface, color);
        }
        else
        {
            er = ClearMultiBackSurface(back_surface, color);
        }
        if (er) return er;
    }
    if (depth_surface)
    {
        er = ClearDepthStencilSurface(depth_surface, depth_value, stencil_value);
        if (er) return er;
    }
    return er;
}

error GraphicAPIDx11::BindBackSurface(const Graphics::IBackSurfacePtr& back_surface, 
    const Graphics::IDepthStencilSurfacePtr& depth_surface)
{
    bool isMultiSurface = false;
    if (back_surface)
    {
        isMultiSurface = back_surface->IsMultiSurface();
    }
    else if (m_boundBackSurface)
    {
        isMultiSurface = m_boundBackSurface->IsMultiSurface();
    }
    if (!isMultiSurface)
    {
        return BindSingleBackSurface(back_surface, depth_surface);
    }
    else
    {
        return BindMultiBackSurface(back_surface, depth_surface);
    }
}

error GraphicAPIDx11::BindViewPort(const Graphics::TargetViewPort& vp)
{
    assert(m_d3dDeviceContext);
    //if (m_kCurrentViewPort==vp) return;
    m_boundViewPort = vp;

    D3D11_VIEWPORT d3dvp;
    d3dvp.Width = (FLOAT)m_boundViewPort.Width();
    d3dvp.Height = (FLOAT)m_boundViewPort.Height();
    d3dvp.MinDepth = m_boundViewPort.MinZ();
    d3dvp.MaxDepth = m_boundViewPort.MaxZ();
    d3dvp.TopLeftX = (FLOAT)m_boundViewPort.X();
    d3dvp.TopLeftY = (FLOAT)m_boundViewPort.Y();
    m_d3dDeviceContext->RSSetViewports(1, &d3dvp);
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateVertexShader(const std::string& name)
{
    Platforms::Debug::Printf("create vertex shader in thread %d\n", std::this_thread::get_id());
    Graphics::IVertexShaderPtr shader = Graphics::IVertexShaderPtr{ menew VertexShaderDx11{ name } };
    m_stash->Add(name, shader);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceVertexShaderCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreatePixelShader(const std::string& name)
{
    Platforms::Debug::Printf("create pixel shader in thread %d\n", std::this_thread::get_id());
    Graphics::IPixelShaderPtr shader = Graphics::IPixelShaderPtr{ menew PixelShaderDx11{ name } };
    m_stash->Add(name, shader);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DevicePixelShaderCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateShaderProgram(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
    const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl)
{
    Platforms::Debug::Printf("create shader program in thread %d\n", std::this_thread::get_id());
    Graphics::IShaderProgramPtr shader = Graphics::IShaderProgramPtr{
        menew ShaderProgramDx11{ name, vtx_shader, pix_shader, vtx_decl } };
    m_stash->Add(name, shader);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceShaderProgramCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
    const Graphics::IVertexShaderPtr& shader)
{
    assert(shader);

    Platforms::Debug::Printf("create vertex declaration in thread %d\n", std::this_thread::get_id());

    auto shader_dx11 = std::dynamic_pointer_cast<VertexShaderDx11, Graphics::IVertexShader>(shader);
    assert(shader_dx11);
    VertexDeclarationDx11* vtx_decl_dx11 = menew
        VertexDeclarationDx11(name, data_vertex_format, shader_dx11->GetShaderVertexFormat());
    vtx_decl_dx11->FillShaderVertexFormat(shader_dx11);
    error er = vtx_decl_dx11->CreateD3DInputLayout(m_d3dDevice);
    if (FATAL_LOG_EXPR(er != ErrorCode::ok))
    {
        Platforms::Debug::ErrorPrintf("Create D3DInputLayout Fail %s", er.message().c_str());
        medelete vtx_decl_dx11;
        return er;
    }

    Graphics::IVertexDeclarationPtr vtxDecl = Graphics::IVertexDeclarationPtr{ vtx_decl_dx11 };
    m_stash->Add(name, vtxDecl);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceVertexDeclarationCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer)
{
    Platforms::Debug::Printf("create vertex buffer in thread %d\n", std::this_thread::get_id());
    Graphics::IVertexBufferPtr buff = Graphics::IVertexBufferPtr{ menew VertexBufferDx11{ buff_name } };
    buff->Create(sizeofVertex, sizeBuffer);
    m_stash->Add(buff_name, buff);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceVertexBufferCreated>(buff_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer)
{
    Platforms::Debug::Printf("create index buffer in thread %d\n", std::this_thread::get_id());
    Graphics::IIndexBufferPtr buff = Graphics::IIndexBufferPtr{ menew IndexBufferDx11{ buff_name } };
    buff->Create(sizeBuffer);
    m_stash->Add(buff_name, buff);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceIndexBufferCreated>(buff_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateSamplerState(const std::string& name, const Graphics::IDeviceSamplerState::SamplerStateData& data)
{
    Platforms::Debug::Printf("create sampler state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceSamplerStatePtr state = Graphics::IDeviceSamplerStatePtr{ menew DeviceSamplerStateDx11{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceSamplerStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateRasterizerState(const std::string& name, const Graphics::IDeviceRasterizerState::RasterizerStateData& data)
{
    Platforms::Debug::Printf("create rasterizer state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceRasterizerStatePtr state = Graphics::IDeviceRasterizerStatePtr{ menew DeviceRasterizerStateDx11{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceRasterizerStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateAlphaBlendState(const std::string& name, const Graphics::IDeviceAlphaBlendState::BlendStateData& data)
{
    Platforms::Debug::Printf("create alpha blend state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceAlphaBlendStatePtr state = Graphics::IDeviceAlphaBlendStatePtr{ menew DeviceAlphaBlendStateDx11{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceAlphaBlendStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateDepthStencilState(const std::string& name, const Graphics::IDeviceDepthStencilState::DepthStencilData& data)
{
    Platforms::Debug::Printf("create depth stencil state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceDepthStencilStatePtr state = Graphics::IDeviceDepthStencilStatePtr{ menew DeviceDepthStencilStateDx11{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceDepthStencilStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateTexture(const std::string& tex_name)
{
    Platforms::Debug::Printf("create texture in thread %d\n", std::this_thread::get_id());
    Graphics::ITexturePtr tex = Graphics::ITexturePtr{ menew TextureDx11{ tex_name } };
    m_stash->Add(tex_name, tex);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceTextureCreated>(tex_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::CreateMultiTexture(const std::string& tex_name)
{
    Platforms::Debug::Printf("create multi-texture in thread %d\n", std::this_thread::get_id());
    Graphics::ITexturePtr tex = Graphics::ITexturePtr{ menew MultiTextureDx11{ tex_name } };
    m_stash->Add(tex_name, tex);

    Frameworks::EventPublisher::Post(std::make_shared<Graphics::DeviceMultiTextureCreated>(tex_name));
    return ErrorCode::ok;
}

error GraphicAPIDx11::BindVertexDeclaration(const Graphics::IVertexDeclarationPtr& vertexDecl)
{
    //Platforms::Debug::Printf("bind vertex declaration in thread %d\n", std::this_thread::get_id());
    assert(m_d3dDeviceContext);
    if (m_boundVertexDecl == vertexDecl) return ErrorCode::ok;
    if ((vertexDecl == nullptr) && (m_boundVertexDecl != nullptr))
    {
        m_d3dDeviceContext->IASetInputLayout(nullptr);
        m_boundVertexDecl = nullptr;
        return ErrorCode::ok;
    }
    auto vtxDx11 = std::dynamic_pointer_cast<VertexDeclarationDx11, Graphics::IVertexDeclaration>(vertexDecl);
    if (FATAL_LOG_EXPR(!vtxDx11)) return ErrorCode::dynamicCastVertexLayout;
    if (FATAL_LOG_EXPR(!vtxDx11->GetD3DInputLayout())) return ErrorCode::nullVertexLayout;
    m_d3dDeviceContext->IASetInputLayout(vtxDx11->GetD3DInputLayout());
    m_boundVertexDecl = vertexDecl;

    return ErrorCode::ok;
}

error GraphicAPIDx11::BindVertexShader(const Graphics::IVertexShaderPtr& shader)
{
    //Platforms::Debug::Printf("bind vertex shader in thread %d\n", std::this_thread::get_id());
    assert(m_d3dDeviceContext);
    if (m_boundVertexShader == shader) return ErrorCode::ok;
    if ((shader == nullptr) && (m_boundVertexShader != nullptr))
    {
        m_d3dDeviceContext->VSSetShader(nullptr, nullptr, 0);
        m_boundVertexShader = nullptr;
        return ErrorCode::ok;
    }
    auto shaderDx11 = std::dynamic_pointer_cast<VertexShaderDx11, Graphics::IVertexShader>(shader);
    if (FATAL_LOG_EXPR(!shaderDx11)) return ErrorCode::dynamicCastShader;
    if (FATAL_LOG_EXPR(!shaderDx11->GetD3DShader())) return ErrorCode::nullShader;
    m_d3dDeviceContext->VSSetShader(shaderDx11->GetD3DShader(), NULL, 0);
    m_boundVertexShader = shader;

    return ErrorCode::ok;
}

error GraphicAPIDx11::BindPixelShader(const Graphics::IPixelShaderPtr& shader)
{
    //Platforms::Debug::Printf("bind pixel shader in thread %d\n", std::this_thread::get_id());
    assert(m_d3dDeviceContext);
    if (m_boundPixelShader == shader) return ErrorCode::ok;
    if ((shader == nullptr) && (m_boundPixelShader != nullptr))
    {
        m_d3dDeviceContext->PSSetShader(nullptr, nullptr, 0);
        m_boundPixelShader = nullptr;
        return ErrorCode::ok;
    }
    auto shaderDx11 = std::dynamic_pointer_cast<PixelShaderDx11, Graphics::IPixelShader>(shader);
    if (FATAL_LOG_EXPR(!shaderDx11)) return ErrorCode::dynamicCastShader;
    if (FATAL_LOG_EXPR(!shaderDx11->GetD3DShader())) return ErrorCode::nullShader;
    m_d3dDeviceContext->PSSetShader(shaderDx11->GetD3DShader(), NULL, 0);
    m_boundPixelShader = shader;

    return ErrorCode::ok;
}

error GraphicAPIDx11::BindShaderProgram(const Graphics::IShaderProgramPtr& shader)
{
    if (m_boundShaderProgram == shader) return ErrorCode::ok;
    error er = BindVertexDeclaration(shader->GetVertexDeclaration());
    if (er) return er;
    er = BindVertexShader(shader->GetVertexShader());
    if (er) return er;
    er = BindPixelShader(shader->GetPixelShader());
    return er;
}

error GraphicAPIDx11::BindVertexBuffer(const Graphics::IVertexBufferPtr& buffer, Graphics::PrimitiveTopology pt)
{
    //Platforms::Debug::Printf("bind vertex buffer in thread %d\n", std::this_thread::get_id());
    assert(m_d3dDeviceContext);
    if ((m_boundVertexBuffer == buffer) && (m_boundTopology == pt)) return ErrorCode::ok;
    if (buffer == nullptr)
    {
        m_boundVertexBuffer = nullptr;
        return ErrorCode::ok;
    }
    auto buffDx11 = std::dynamic_pointer_cast<VertexBufferDx11, Graphics::IVertexBuffer>(buffer);
    if (FATAL_LOG_EXPR(!buffDx11)) return ErrorCode::dynamicCastBuffer;
    if (FATAL_LOG_EXPR(!buffDx11->GetD3DBuffer())) return ErrorCode::nullVertexBuffer;
    ID3D11Buffer* d3dBuffer = buffDx11->GetD3DBuffer();
    unsigned int offset = 0;
    unsigned int sizeVertex = buffer->SizeofVertex();
    m_d3dDeviceContext->IASetVertexBuffers(0, 1, &d3dBuffer, &sizeVertex, &offset);
    m_d3dDeviceContext->IASetPrimitiveTopology(ConvertTopologyD3D11(pt));
    m_boundVertexBuffer = buffer;
    m_boundTopology = pt;

    return ErrorCode::ok;
}

error GraphicAPIDx11::BindIndexBuffer(const Graphics::IIndexBufferPtr& buffer)
{
    //Platforms::Debug::Printf("bind index buffer in thread %d\n", std::this_thread::get_id());
    assert(m_d3dDeviceContext);
    if (m_boundIndexBuffer == buffer) return ErrorCode::ok;
    if (buffer == nullptr)
    {
        m_boundIndexBuffer = nullptr;
        return ErrorCode::ok;
    }
    auto buffDx11 = std::dynamic_pointer_cast<IndexBufferDx11, Graphics::IIndexBuffer>(buffer);
    if (FATAL_LOG_EXPR(!buffDx11)) return ErrorCode::dynamicCastBuffer;
    if (FATAL_LOG_EXPR(!buffDx11->GetD3DBuffer())) return ErrorCode::nullIndexBuffer;
    m_d3dDeviceContext->IASetIndexBuffer(buffDx11->GetD3DBuffer(), DXGI_FORMAT_R32_UINT, 0);
    m_boundIndexBuffer = buffer;

    return ErrorCode::ok;
}

void GraphicAPIDx11::CleanupDeviceObjects()
{
    m_boundVertexDecl = nullptr;
    m_boundVertexShader = nullptr;
    m_boundPixelShader = nullptr;
    m_boundShaderProgram = nullptr;
    m_boundVertexBuffer = nullptr;
    m_boundIndexBuffer = nullptr;
    m_boundBackSurface = nullptr;
    m_boundDepthSurface = nullptr;
}

error GraphicAPIDx11::ClearSingleBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color)
{
    if (FATAL_LOG_EXPR(!m_d3dDeviceContext)) return ErrorCode::d3dDeviceNullPointer;
    if (back_surface)
    {
        auto back_dx11 = std::dynamic_pointer_cast<BackSurfaceDx11, Graphics::IBackSurface>(back_surface);
        if (!back_dx11) return ErrorCode::dynamicCastSurface;

        m_d3dDeviceContext->ClearRenderTargetView(back_dx11->GetD3DRenderTarget(), (const float*)color);
    }
    return ErrorCode::ok;
}

error GraphicAPIDx11::ClearMultiBackSurface(const Graphics::IBackSurfacePtr& back_surface, const MathLib::ColorRGBA& color)
{
    if (FATAL_LOG_EXPR(!m_d3dDeviceContext)) return ErrorCode::d3dDeviceNullPointer;
    if (back_surface)
    {
        auto back_dx11 = std::dynamic_pointer_cast<MultiBackSurfaceDx11, Graphics::IBackSurface>(back_surface);
        if (!back_dx11) return ErrorCode::dynamicCastSurface;

        for (unsigned int i = 0; i < back_surface->GetSurfaceCount(); i++)
        {
            m_d3dDeviceContext->ClearRenderTargetView(back_dx11->GetD3DRenderTarget(i), (const float*)color);
        }
    }
    return ErrorCode::ok;
}

error GraphicAPIDx11::ClearDepthStencilSurface(const Graphics::IDepthStencilSurfacePtr& depth_surface,
    float depth_value, unsigned int stencil_value)
{
    if (FATAL_LOG_EXPR(!m_d3dDeviceContext)) return ErrorCode::d3dDeviceNullPointer;
    if (depth_surface)
    {
        unsigned int flag = 0;
        if (depth_surface->GetFormat().DepthBits()) flag |= D3D11_CLEAR_DEPTH;
        if (depth_surface->GetFormat().StencilBits()) flag |= D3D11_CLEAR_STENCIL;
        auto depth_dx11 = std::dynamic_pointer_cast<DepthStencilSurfaceDx11, Graphics::IDepthStencilSurface>(depth_surface);
        if (!depth_dx11) return ErrorCode::dynamicCastSurface;
        m_d3dDeviceContext->ClearDepthStencilView(
            depth_dx11->GetD3DDepthView(), (D3D11_CLEAR_FLAG)flag, depth_value, (UINT8)stencil_value);
    }
    return ErrorCode::ok;
}

error GraphicAPIDx11::BindSingleBackSurface(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface)
{
    assert(m_d3dDeviceContext);
    if ((m_boundBackSurface == back_surface) && (m_boundDepthSurface == depth_surface)) return ErrorCode::ok;
    m_boundBackSurface = back_surface;
    m_boundDepthSurface = depth_surface;
    if (m_boundBackSurface == nullptr)
    {
        ID3D11RenderTargetView* target = nullptr;
        m_d3dDeviceContext->OMSetRenderTargets(1, &target, nullptr);
        return ErrorCode::ok;
    }

    auto bbDx11 = std::dynamic_pointer_cast<BackSurfaceDx11, Graphics::IBackSurface>(m_boundBackSurface);
    if (FATAL_LOG_EXPR(!bbDx11)) return ErrorCode::dynamicCastSurface;
    ID3D11RenderTargetView* render_target_view = bbDx11->GetD3DRenderTarget();
    ID3D11DepthStencilView* depth_view = nullptr;
    if (m_boundDepthSurface)
    {
        auto dsbDx11 = std::dynamic_pointer_cast<DepthStencilSurfaceDx11, Graphics::IDepthStencilSurface>(m_boundDepthSurface);
        if (dsbDx11) depth_view = dsbDx11->GetD3DDepthView();
    }
    m_d3dDeviceContext->OMSetRenderTargets(1, &render_target_view, depth_view);

    return ErrorCode::ok;
}

error GraphicAPIDx11::BindMultiBackSurface(const Graphics::IBackSurfacePtr& back_surface, const Graphics::IDepthStencilSurfacePtr& depth_surface)
{
    assert(m_d3dDeviceContext);
    if ((m_boundBackSurface == back_surface) && (m_boundDepthSurface == depth_surface)) return ErrorCode::ok;
    unsigned int bound_sourface_count = 1;
    if (m_boundBackSurface) bound_sourface_count = m_boundBackSurface->GetSurfaceCount();
    m_boundBackSurface = back_surface;
    m_boundDepthSurface = depth_surface;
    if (m_boundBackSurface == nullptr)
    {
        ID3D11RenderTargetView* target = nullptr;
        m_d3dDeviceContext->OMSetRenderTargets(bound_sourface_count, &target, nullptr);
        return ErrorCode::ok;
    }

    auto bbDx11 = std::dynamic_pointer_cast<MultiBackSurfaceDx11, Graphics::IBackSurface>(m_boundBackSurface);
    if (FATAL_LOG_EXPR(!bbDx11)) return ErrorCode::dynamicCastSurface;
    ID3D11RenderTargetView** render_target_view = bbDx11->GetD3DRenderTargetArray();
    ID3D11DepthStencilView* depth_view = nullptr;
    if (m_boundDepthSurface)
    {
        auto dsbDx11 = std::dynamic_pointer_cast<DepthStencilSurfaceDx11, Graphics::IDepthStencilSurface>(m_boundDepthSurface);
        if (dsbDx11) depth_view = dsbDx11->GetD3DDepthView();
    }
    m_d3dDeviceContext->OMSetRenderTargets(back_surface->GetSurfaceCount(), render_target_view, depth_view);

    return ErrorCode::ok;
}

void GraphicAPIDx11::AddDebugInfoFilter()
{
    ID3D11Debug* d3dDebug = nullptr;
    if (SUCCEEDED(m_d3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {

            D3D11_MESSAGE_ID hide[] =
            {
                D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
                D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
                // Add more message IDs here as needed
            };

            D3D11_INFO_QUEUE_FILTER filter;
            memset(&filter, 0, sizeof(filter));
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
}

ID3D11Texture2D* GraphicAPIDx11::GetPrimaryD3DBackbuffer()
{
    if (FATAL_LOG_EXPR((!m_swapChain) || (!m_swapChain->GetDXGIObject()))) return nullptr;
    ID3D11Texture2D* back_surface;
    HRESULT hr = m_swapChain->GetDXGIObject()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_surface);
    if (FAILED(hr))
    {
        return nullptr;
    }
    return back_surface;
}

//----------------------------------------------------------------------------
DXGI_FORMAT ConvertGraphicFormatToDXGI(const Enigma::Graphics::GraphicFormat& format)
{
    switch (format.fmt)
    {
    case Enigma::Graphics::GraphicFormat::FMT_UNKNOWN:        return DXGI_FORMAT_UNKNOWN;
    case Enigma::Graphics::GraphicFormat::FMT_R8G8B8:
    case Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8:
    case Enigma::Graphics::GraphicFormat::FMT_X8R8G8B8:       return DXGI_FORMAT_B8G8R8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_R5G6B5:         return DXGI_FORMAT_B5G6R5_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_X1R5G5B5:
    case Enigma::Graphics::GraphicFormat::FMT_A1R5G5B5:       return DXGI_FORMAT_B5G5R5A1_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A4R4G4B4:       return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_R3G3B2:         return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A8:             return DXGI_FORMAT_A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A8R3G3B2:       return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_X4R4G4B4:       return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A2B10G10R10:    return DXGI_FORMAT_R10G10B10A2_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A8B8G8R8:
    case Enigma::Graphics::GraphicFormat::FMT_X8B8G8R8:       return DXGI_FORMAT_R8G8B8A8_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_G16R16:         return DXGI_FORMAT_R16G16_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A2R10G10B10:    return DXGI_FORMAT_R10G10B10A2_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16:   return DXGI_FORMAT_R16G16B16A16_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_R16F:           return DXGI_FORMAT_R16_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_G16R16F:        return DXGI_FORMAT_R16G16_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16F:  return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_R32F:           return DXGI_FORMAT_R32_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_G32R32F:        return DXGI_FORMAT_R32G32_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_A32B32G32R32F:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_D16_LOCKABLE:
    case Enigma::Graphics::GraphicFormat::FMT_D16:            return DXGI_FORMAT_D16_UNORM;
    case Enigma::Graphics::GraphicFormat::FMT_D32F_LOCKABLE:
    case Enigma::Graphics::GraphicFormat::FMT_D32:            return DXGI_FORMAT_D32_FLOAT;
    case Enigma::Graphics::GraphicFormat::FMT_D24X8:
    case Enigma::Graphics::GraphicFormat::FMT_D24X4S4:
    case Enigma::Graphics::GraphicFormat::FMT_D24FS8:
    case Enigma::Graphics::GraphicFormat::FMT_D24S8:           return DXGI_FORMAT_D24_UNORM_S8_UINT;
    }
    return DXGI_FORMAT_UNKNOWN;
}

unsigned int ConvertDXGIFormatToGraphicFormat(DXGI_FORMAT fmt)
{
    switch (fmt)
    {
    case DXGI_FORMAT_UNKNOWN:               return Enigma::Graphics::GraphicFormat::FMT_UNKNOWN;
    case DXGI_FORMAT_R8G8B8A8_UNORM:        return Enigma::Graphics::GraphicFormat::FMT_A8B8G8R8;
    case DXGI_FORMAT_B5G6R5_UNORM:          return Enigma::Graphics::GraphicFormat::FMT_R5G6B5;
    case DXGI_FORMAT_B5G5R5A1_UNORM:        return Enigma::Graphics::GraphicFormat::FMT_A1R5G5B5;
    case DXGI_FORMAT_A8_UNORM:              return Enigma::Graphics::GraphicFormat::FMT_A8;
    case DXGI_FORMAT_R10G10B10A2_UNORM:     return Enigma::Graphics::GraphicFormat::FMT_A2B10G10R10;
    case DXGI_FORMAT_B8G8R8A8_UNORM:        return Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8;
    case DXGI_FORMAT_R16G16_UNORM:          return Enigma::Graphics::GraphicFormat::FMT_G16R16;
    case DXGI_FORMAT_R16G16B16A16_UNORM:    return Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16;
    case DXGI_FORMAT_R16_FLOAT:             return Enigma::Graphics::GraphicFormat::FMT_R16F;
    case DXGI_FORMAT_R16G16_FLOAT:          return Enigma::Graphics::GraphicFormat::FMT_G16R16F;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:    return Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16F;
    case DXGI_FORMAT_R32_FLOAT:             return Enigma::Graphics::GraphicFormat::FMT_R32F;
    case DXGI_FORMAT_R32G32_FLOAT:          return Enigma::Graphics::GraphicFormat::FMT_G32R32F;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:    return Enigma::Graphics::GraphicFormat::FMT_A32B32G32R32F;
    case DXGI_FORMAT_D16_UNORM:             return Enigma::Graphics::GraphicFormat::FMT_D16;
    case DXGI_FORMAT_D32_FLOAT:             return Enigma::Graphics::GraphicFormat::FMT_D32;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:     return Enigma::Graphics::GraphicFormat::FMT_D24S8;
    default: return Enigma::Graphics::GraphicFormat::FMT_UNKNOWN;
    }
}
