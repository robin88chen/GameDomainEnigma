#include "GraphicAPIEgl.h"
#include "BackSurfaceEgl.h"
#include "DepthStencilSurfaceEgl.h"
#include "MultiBackSurfaceEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformLayer.h"
#include "MathLib/ColorRGBA.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

using namespace Enigma::Devices;
using namespace Enigma::Platforms;
using ErrorCode = Enigma::Graphics::ErrorCode;

GraphicAPIEgl::GraphicAPIEgl() : IGraphicAPI()
{
    m_apiVersion = APIVersion::API_EGL;
    m_fmtBackSurface = m_fmtDepthSurface = Graphics::GraphicFormat(Graphics::GraphicFormat::FMT_UNKNOWN);
}

GraphicAPIEgl::~GraphicAPIEgl()
{
    
}

error GraphicAPIEgl::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    m_wnd = hwnd;
    m_deviceRequiredBits = rqb;
    const char* versionStr = (const char*)glGetString(GL_VERSION);
    Debug::Printf("Version %s", versionStr);
    const char* extentStr = (const char*)glGetString(GL_EXTENSIONS);
    Debug::Printf("Extensions %s", extentStr);
    return ErrorCode::ok;
}

error GraphicAPIEgl::CleanupDevice()
{
    CleanupDeviceObjects();
    return ErrorCode::ok;
}

error GraphicAPIEgl::BeginScene()
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::EndScene()
{
    return ErrorCode::ok;
}

void GraphicAPIEgl::CleanupDeviceObjects()
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

error GraphicAPIEgl::DrawPrimitive(unsigned vertexCount, unsigned vertexOffset)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::DrawIndexedPrimitive(unsigned indexCount, unsigned vertexCount, unsigned indexOffset, int baseVertexOffset)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::Flip()
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    Debug::Printf("create primary back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceEgl{ back_name, m_surfaceDimension, GetPrimaryBackSurfaceFormat(), true } };
    m_repository->Add(back_name, back_surface);

    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, m_surfaceDimension, GetDepthSurfaceFormat() } };
    m_repository->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::PrimarySurfaceCreated{ back_name, depth_name} });

    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    Debug::Printf("create back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceEgl{ back_name, dimension, fmt, false} };
    m_repository->Add(back_name, back_surface);

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::BackSurfaceCreated{ back_name } });
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    unsigned buff_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    Debug::Printf("create multi back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew MultiBackSurfaceEgl{ back_name, dimension, buff_count, fmts } };
    m_repository->Add(back_name, back_surface);

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::MultiBackSurfaceCreated{ back_name } });
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, dimension, fmt } };
    m_repository->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::DepthSurfaceCreated{ depth_name } });
    return ErrorCode::ok;
}

error GraphicAPIEgl::ShareDepthStencilSurface(const std::string& depth_name,
    const Graphics::IDepthStencilSurfacePtr& from_depth)
{
    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    DepthStencilSurfaceEgl* depth_egl = dynamic_cast<DepthStencilSurfaceEgl*>(from_depth.get());
    assert(depth_egl);
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, depth_egl } };
    m_repository->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::Post(Frameworks::IEventPtr{ menew Graphics::DepthSurfaceShared{ depth_name } });
    return ErrorCode::ok;
}

error GraphicAPIEgl::ClearSurface(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    assert(back_surface);
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    // clear 之前，要先把 mask 設好，不然不會清 (很大的 trick)
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(color.R(), color.G(), color.B(), color.A());
    if (depth_surface)
    {
        mask |= (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDepthMask(GL_TRUE);
        glClearDepthf(depth_value);
        glStencilMask(0xff);
        glClearStencil(stencil_value);
    }
    glClear(mask);
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindBackSurface(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface)
{
    if ((m_boundBackSurface == back_surface) && (m_boundDepthSurface == depth_surface)) return ErrorCode::ok;
    assert(back_surface);
    m_boundBackSurface = back_surface;
    m_boundDepthSurface = depth_surface;
    if (back_surface->IsPrimary())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else if (!back_surface->IsMultiSurface())
    {
        BackSurfaceEgl* surface_egl = dynamic_cast<BackSurfaceEgl*>(back_surface.get());
        assert(surface_egl);
        if (surface_egl->GetFrameBufferHandle() != 0) glBindFramebuffer(GL_FRAMEBUFFER, surface_egl->GetFrameBufferHandle());
    }
    else
    {
        MultiBackSurfaceEgl* surface_egl = dynamic_cast<MultiBackSurfaceEgl*>(back_surface.get());
        assert(surface_egl);
        if (surface_egl->GetFrameBufferHandle() != 0) glBindFramebuffer(GL_FRAMEBUFFER, surface_egl->GetFrameBufferHandle());
    }
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindViewPort(const Graphics::TargetViewPort& vp)
{
    if (m_boundViewPort == vp) return ErrorCode::ok;
    m_boundViewPort = vp;

    glViewport(m_boundViewPort.X(), m_boundViewPort.Y(),
        m_boundViewPort.Width(), m_boundViewPort.Height());
    glDepthRangef(m_boundViewPort.MinZ(), m_boundViewPort.MaxZ());

    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexShader(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreatePixelShader(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateShaderProgram(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
                                         const Graphics::IPixelShaderPtr& pix_shader)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
    const Graphics::IVertexShaderPtr& shader)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexBuffer(const std::string& buff_name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateIndexBuffer(const std::string& buff_name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateSamplerState(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateRasterizerState(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateAlphaBlendState(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateDepthStencilState(const std::string& name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateTexture(const std::string& tex_name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateMultiTexture(const std::string& tex_name)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindVertexDeclaration(const Graphics::IVertexDeclarationPtr& vertexDecl)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindVertexShader(const Graphics::IVertexShaderPtr& shader)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindPixelShader(const Graphics::IPixelShaderPtr& shader)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindShaderProgram(const Graphics::IShaderProgramPtr& shader)
{
    return ErrorCode::ok;
}

future_error GraphicAPIEgl::AsyncBindShaderProgram(const Graphics::IShaderProgramPtr& shader)
{
    return make_future_err(ErrorCode::ok);
}

error GraphicAPIEgl::BindVertexBuffer(const Graphics::IVertexBufferPtr& buffer, Graphics::PrimitiveTopology pt)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindIndexBuffer(const Graphics::IIndexBufferPtr& buffer)
{
    return ErrorCode::ok;
}

void GraphicAPIEgl::SetFormat(int* attrb)
{
    assert(attrb);
    m_fmtBackSurface = Graphics::GraphicFormat::ColorFormat(attrb[0], attrb[1], attrb[2], attrb[3]);
    m_fmtDepthSurface = Graphics::GraphicFormat::DepthFormat(attrb[4], attrb[5]);
    if (m_fmtDepthSurface.DepthBits() > 0)
    {
        glEnable(GL_DEPTH_TEST);
    }
}

void GraphicAPIEgl::SetDimension(const MathLib::Dimension& dim)
{
    m_surfaceDimension = dim;
}
