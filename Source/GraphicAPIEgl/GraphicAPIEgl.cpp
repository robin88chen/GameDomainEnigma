#include "GraphicAPIEgl.h"
#include "BackSurfaceEgl.h"
#include "DepthStencilSurfaceEgl.h"
#include "DeviceSamplerStateEgl.h"
#include "MultiBackSurfaceEgl.h"
#include "TextureEgl.h"
#include "MultiTextureEgl.h"
#include "VertexBufferEgl.h"
#include "IndexBufferEgl.h"
#include "VertexShaderEgl.h"
#include "PixelShaderEgl.h"
#include "ShaderProgramEgl.h"
#include "VertexDeclarationEgl.h"
#include "DeviceAlphaBlendStateEgl.h"
#include "DeviceDepthStencilStateEgl.h"
#include "DeviceRasterizerStateEgl.h"
#include "Frameworks/EventPublisher.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "GraphicKernel/GraphicThread.h"
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

GLenum PrimitiveTopologyToGL(Enigma::Graphics::PrimitiveTopology pt);

GraphicAPIEgl::GraphicAPIEgl() : IGraphicAPI(AsyncType::NotAsyncDevice), m_surfaceDimension{ 1, 1 }
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
    const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    Debug::Printf("Version %s", versionStr);
    const char* extentStr = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    Debug::Printf("Extensions %s", extentStr);
    return ErrorCode::ok;
}

error GraphicAPIEgl::CleanupDevice()
{
    CleanupDeviceObjects();
    return ErrorCode::ok;
}

error GraphicAPIEgl::BeginDrawingScene()
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::EndDrawingScene()
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
    glDrawArrays(PrimitiveTopologyToGL(m_boundTopology), static_cast<GLint>(vertexOffset), static_cast<GLsizei>(vertexCount));
    return ErrorCode::ok;
}

error GraphicAPIEgl::DrawIndexedPrimitive(unsigned indexCount, unsigned vertexCount, unsigned indexOffset, int baseVertexOffset)
{
    // GLES 3.0 才有
    glDrawRangeElements(PrimitiveTopologyToGL(m_boundTopology), baseVertexOffset, baseVertexOffset + vertexCount - 1,
        static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    return ErrorCode::ok;
}

error GraphicAPIEgl::FlipBackSurface()
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    Debug::Printf("create primary back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceEgl{ back_name, m_surfaceDimension, GetPrimaryBackSurfaceFormat(), true } };
    m_stash->Add(back_name, back_surface);

    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, m_surfaceDimension, GetDepthSurfaceFormat() } };
    m_stash->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::PrimarySurfaceCreated>(back_name, depth_name));

    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    const Graphics::GraphicFormat& fmt)
{
    Debug::Printf("create back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew BackSurfaceEgl{ back_name, dimension, fmt, false} };
    m_stash->Add(back_name, back_surface);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::BackSurfaceCreated>(back_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    unsigned buff_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    Debug::Printf("create multi back surface in thread %d\n", std::this_thread::get_id());
    Graphics::IBackSurfacePtr back_surface = Graphics::IBackSurfacePtr{
        menew MultiBackSurfaceEgl{ back_name, dimension, buff_count, fmts } };
    m_stash->Add(back_name, back_surface);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::MultiBackSurfaceCreated>(back_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension,
    const Graphics::GraphicFormat& fmt)
{
    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, dimension, fmt } };
    m_stash->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DepthSurfaceCreated>(depth_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::ShareDepthStencilSurface(const std::string& depth_name,
    const Graphics::IDepthStencilSurfacePtr& from_depth)
{
    Debug::Printf("create depth surface in thread %d\n", std::this_thread::get_id());
    auto depth_egl = std::dynamic_pointer_cast<DepthStencilSurfaceEgl, Graphics::IDepthStencilSurface>(from_depth);
    assert(depth_egl);
    Graphics::IDepthStencilSurfacePtr depth_surface = Graphics::IDepthStencilSurfacePtr{
        menew DepthStencilSurfaceEgl{ depth_name, depth_egl } };
    m_stash->Add(depth_name, depth_surface);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DepthSurfaceShared>(depth_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::ClearSurface(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    assert(back_surface);
    // 檢查是不是 bounded
    if ((m_boundBackSurface != back_surface) || (m_boundDepthSurface != depth_surface)) return ErrorCode::surfaceNotBound;

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
        glClearStencil(static_cast<GLint>(stencil_value));
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
        auto surface_egl = std::dynamic_pointer_cast<BackSurfaceEgl, Graphics::IBackSurface>(back_surface);
        assert(surface_egl);
        if (surface_egl->GetFrameBufferHandle() != 0) glBindFramebuffer(GL_FRAMEBUFFER, surface_egl->GetFrameBufferHandle());
    }
    else
    {
        auto surface_egl = std::dynamic_pointer_cast<MultiBackSurfaceEgl, Graphics::IBackSurface>(back_surface);
        assert(surface_egl);
        if (surface_egl->GetFrameBufferHandle() != 0) glBindFramebuffer(GL_FRAMEBUFFER, surface_egl->GetFrameBufferHandle());
    }
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindViewPort(const Graphics::TargetViewPort& vp)
{
    if (m_boundViewPort == vp) return ErrorCode::ok;
    m_boundViewPort = vp;

    glViewport(static_cast<GLint>(m_boundViewPort.X()), static_cast<GLint>(m_boundViewPort.Y()),
        static_cast<GLsizei>(m_boundViewPort.Width()), static_cast<GLsizei>(m_boundViewPort.Height()));
    glDepthRangef(m_boundViewPort.MinZ(), m_boundViewPort.MaxZ());

    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexShader(const std::string& name)
{
    Debug::Printf("create vertex shader in thread %d\n", std::this_thread::get_id());
    Graphics::IVertexShaderPtr shader = Graphics::IVertexShaderPtr{ menew VertexShaderEgl{ name } };
    m_stash->Add(name, shader);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceVertexShaderCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreatePixelShader(const std::string& name)
{
    Debug::Printf("create pixel shader in thread %d\n", std::this_thread::get_id());
    Graphics::IPixelShaderPtr shader = Graphics::IPixelShaderPtr{ menew PixelShaderEgl{ name } };
    m_stash->Add(name, shader);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DevicePixelShaderCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateShaderProgram(const std::string& name, const Graphics::IVertexShaderPtr& vtx_shader,
    const Graphics::IPixelShaderPtr& pix_shader, const Graphics::IVertexDeclarationPtr& vtx_decl)
{
    Debug::Printf("create shader program in thread %d\n", std::this_thread::get_id());
    Graphics::IShaderProgramPtr shader = Graphics::IShaderProgramPtr{ menew ShaderProgramEgl{ name, vtx_shader, pix_shader, vtx_decl } };
    if (std::dynamic_pointer_cast<ShaderProgramEgl, Graphics::IShaderProgram>(shader)->HasLinked())
    {
        m_stash->Add(name, shader);

        Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceShaderProgramCreated>(name));
    }
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format,
    const Graphics::IVertexShaderPtr& shader)
{
    assert(shader);

    Debug::Printf("create vertex declaration in thread %d\n", std::this_thread::get_id());

    Graphics::IVertexDeclarationPtr vtxDecl = Graphics::IVertexDeclarationPtr{ menew
        VertexDeclarationEgl(name, data_vertex_format) };
    m_stash->Add(name, vtxDecl);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceVertexDeclarationCreated>(name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer)
{
    Debug::Printf("create vertex buffer in thread %d\n", std::this_thread::get_id());
    Graphics::IVertexBufferPtr buff = Graphics::IVertexBufferPtr{ menew VertexBufferEgl{ buff_name } };
    buff->Create(sizeofVertex, sizeBuffer);
    m_stash->Add(buff_name, buff);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceVertexBufferCreated>(buff_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer)
{
    Debug::Printf("create index buffer in thread %d\n", std::this_thread::get_id());
    Graphics::IIndexBufferPtr buff = Graphics::IIndexBufferPtr{ menew IndexBufferEgl{ buff_name } };
    buff->Create(sizeBuffer);
    m_stash->Add(buff_name, buff);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceIndexBufferCreated>(buff_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateSamplerState(const std::string& name, const Graphics::IDeviceSamplerState::SamplerStateData& data)
{
    Debug::Printf("create sampler state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceSamplerStatePtr state = Graphics::IDeviceSamplerStatePtr{ menew DeviceSamplerStateEgl{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceSamplerStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateRasterizerState(const std::string& name, const Graphics::IDeviceRasterizerState::RasterizerStateData& data)
{
    Debug::Printf("create rasterizer state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceRasterizerStatePtr state = Graphics::IDeviceRasterizerStatePtr{ menew DeviceRasterizerStateEgl{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceRasterizerStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateAlphaBlendState(const std::string& name, const Graphics::IDeviceAlphaBlendState::BlendStateData& data)
{
    Debug::Printf("create alpha blend state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceAlphaBlendStatePtr state = Graphics::IDeviceAlphaBlendStatePtr{ menew DeviceAlphaBlendStateEgl{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceAlphaBlendStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateDepthStencilState(const std::string& name, const Graphics::IDeviceDepthStencilState::DepthStencilData& data)
{
    Debug::Printf("create depth stencil state %s in thread %d\n", name.c_str(), std::this_thread::get_id());
    Graphics::IDeviceDepthStencilStatePtr state = Graphics::IDeviceDepthStencilStatePtr{ menew DeviceDepthStencilStateEgl{ name } };
    state->CreateFromData(data);
    //m_stash->Add(name, state);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceDepthStencilStateCreated>(name, state));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateTexture(const std::string& tex_name)
{
    Debug::Printf("create texture in thread %d\n", std::this_thread::get_id());
    Graphics::ITexturePtr tex = Graphics::ITexturePtr{ menew TextureEgl{ tex_name } };
    m_stash->Add(tex_name, tex);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceTextureCreated>(tex_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateMultiTexture(const std::string& tex_name)
{
    Debug::Printf("create multi-texture in thread %d\n", std::this_thread::get_id());
    Graphics::ITexturePtr tex = Graphics::ITexturePtr{ menew MultiTextureEgl{ tex_name } };
    m_stash->Add(tex_name, tex);

    Frameworks::EventPublisher::post(std::make_shared<Graphics::DeviceMultiTextureCreated>(tex_name));
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindVertexDeclaration(const Graphics::IVertexDeclarationPtr& vertexDecl)
{
    // Each generic vertex attribute array is initially disabled and isn't accessed
    // when glDrawElements, glDrawRangeElements, glDrawArrays, glMultiDrawArrays, or glMultiDrawElements is called.
    // 呼叫 glDraw之後，就會失效，所以，每次Draw之前都要Bind, 不能 cache
    if ((m_boundVertexDecl == vertexDecl) && (m_boundVertexDecl != nullptr))
    {
        return BindVertexDeclarationEgl(std::dynamic_pointer_cast<VertexDeclarationEgl, Graphics::IVertexDeclaration>
            (m_boundVertexDecl));
    }

    if (m_boundVertexDecl)
    {
        UnBindVertexDeclarationEgl(std::dynamic_pointer_cast<VertexDeclarationEgl, Graphics::IVertexDeclaration>
            (m_boundVertexDecl));
    }
    if (vertexDecl == nullptr)
    {
        m_boundVertexDecl = nullptr;
        return ErrorCode::ok;
    }
    m_boundVertexDecl = vertexDecl;
    return BindVertexDeclarationEgl(std::dynamic_pointer_cast<VertexDeclarationEgl, Graphics::IVertexDeclaration>
        (m_boundVertexDecl));
}

error GraphicAPIEgl::BindVertexShader(const Graphics::IVertexShaderPtr& shader)
{
    if (m_boundVertexShader == shader) return ErrorCode::ok;
    m_boundVertexShader = shader;
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindPixelShader(const Graphics::IPixelShaderPtr& shader)
{
    if (m_boundPixelShader == shader) return ErrorCode::ok;
    m_boundPixelShader = shader;
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindShaderProgram(const Graphics::IShaderProgramPtr& shader)
{
    //if (m_boundShaderProgram == shader) return ErrorCode::ok; // vertex layout 需要每次 bind, 要略過這個檢查
    auto program_egl = std::dynamic_pointer_cast<ShaderProgramEgl, Graphics::IShaderProgram>(shader);
    assert(program_egl);
    glUseProgram(program_egl->GetProgram());
    m_boundShaderProgram = shader;
    BindVertexDeclaration(shader->GetVertexDeclaration());
    BindVertexShader(shader->GetVertexShader());
    BindPixelShader(shader->GetPixelShader());
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindVertexBuffer(const Graphics::IVertexBufferPtr& buffer, Graphics::PrimitiveTopology pt)
{
    if ((m_boundVertexBuffer == buffer) && (m_boundTopology == pt)) return ErrorCode::ok;
    if (buffer == nullptr)
    {
        m_boundVertexBuffer = nullptr;
        m_boundTopology = pt;
        return ErrorCode::ok;
    }
    m_boundVertexBuffer = buffer;
    m_boundTopology = pt;
    auto buffEgl = std::dynamic_pointer_cast<VertexBufferEgl, Graphics::IVertexBuffer>(buffer);
    if (FATAL_LOG_EXPR(!buffEgl)) return ErrorCode::dynamicCastBuffer;
    if (FATAL_LOG_EXPR(buffEgl->GetBufferHandle() == 0)) return ErrorCode::nullVertexBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, buffEgl->GetBufferHandle());

    return ErrorCode::ok;
}

error GraphicAPIEgl::BindIndexBuffer(const Graphics::IIndexBufferPtr& buffer)
{
    if (m_boundIndexBuffer == buffer) return ErrorCode::ok;
    if (buffer == nullptr)
    {
        m_boundIndexBuffer = nullptr;
        return ErrorCode::ok;
    }
    m_boundIndexBuffer = buffer;
    auto buffEgl = std::dynamic_pointer_cast<IndexBufferEgl, Graphics::IIndexBuffer>(buffer);
    if (FATAL_LOG_EXPR(!buffEgl)) return ErrorCode::dynamicCastBuffer;
    if (FATAL_LOG_EXPR(buffEgl->GetBufferHandle() == 0)) return ErrorCode::nullIndexBuffer;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffEgl->GetBufferHandle());

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

void GraphicAPIEgl::SetDimension(const MathLib::Dimension<unsigned>& dim)
{
    m_surfaceDimension = dim;
}
error GraphicAPIEgl::BindVertexDeclarationEgl(const std::shared_ptr<VertexDeclarationEgl>& vtxDecl)
{
    assert(vtxDecl);
    auto layouts = vtxDecl->GetLayoutElements();
    unsigned int num_elements = vtxDecl->GetElementCount();
    GLsizei vertex_size = vtxDecl->GetVertexSize();
    if (FATAL_LOG_EXPR((layouts.empty()) || (num_elements == 0))) return ErrorCode::vertexLayout;
    for (unsigned int i = 0; i < num_elements; i++)
    {
        glEnableVertexAttribArray(i);
        if (layouts[i].m_type == GL_FLOAT)
        {
            glVertexAttribPointer(i, layouts[i].m_size, layouts[i].m_type, GL_FALSE, vertex_size, reinterpret_cast<const GLvoid*>(layouts[i].m_position));
        }
        else if (layouts[i].m_type == GL_UNSIGNED_INT)
        {
            glVertexAttribIPointer(i, layouts[i].m_size, layouts[i].m_type, vertex_size, reinterpret_cast<const GLvoid*>(layouts[i].m_position));
        }
    }
    return ErrorCode::ok;
}

error GraphicAPIEgl::UnBindVertexDeclarationEgl(const std::shared_ptr<VertexDeclarationEgl>& vtxDecl)
{
    assert(vtxDecl);
    auto layouts = vtxDecl->GetLayoutElements();
    unsigned int num_elements = vtxDecl->GetElementCount();
    if (FATAL_LOG_EXPR((layouts.empty()) || (num_elements == 0))) return ErrorCode::vertexLayout;
    for (unsigned int i = 0; i < num_elements; i++)
    {
        glDisableVertexAttribArray(i);
    }
    return ErrorCode::ok;
}

GLenum PrimitiveTopologyToGL(Enigma::Graphics::PrimitiveTopology pt)
{
    assert(pt != Enigma::Graphics::PrimitiveTopology::Topology_Undefine);
    switch (pt)
    {
    case Enigma::Graphics::PrimitiveTopology::Topology_LineList:
    {
        return GL_LINES;
    }
        case Enigma::Graphics::PrimitiveTopology::Topology_LineStrip:
    {
        return GL_LINE_STRIP;
    }
        case Enigma::Graphics::PrimitiveTopology::Topology_PointList:
    {
        return GL_POINTS;
    }
        case Enigma::Graphics::PrimitiveTopology::Topology_TriangleList:
    {
        return GL_TRIANGLES;
    }
        case Enigma::Graphics::PrimitiveTopology::Topology_TriangleStrip:
    {
        return GL_TRIANGLE_STRIP;
    }
        default:
        break;
    }
    return GL_TRIANGLES;
}

GLint GraphicFormatToGLSizedFormat(const Enigma::Graphics::GraphicFormat& fmt)
{
    switch (fmt.fmt)
    {
        case Enigma::Graphics::GraphicFormat::FMT_A8B8G8R8:
        case Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8:
        {
            return GL_RGBA8;
        }
        case Enigma::Graphics::GraphicFormat::FMT_R8G8B8:
        {
            return GL_RGB8;
        }
        case Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16F:
        {
            return GL_RGBA16F;
        }
        case Enigma::Graphics::GraphicFormat::FMT_R32F:
        {
            return GL_R32F;
        }
    }
    return GL_RGBA8;
}

GLenum GraphicFormatToGLFormat(const Enigma::Graphics::GraphicFormat& fmt)
{
    switch (fmt.fmt)
    {
        case Enigma::Graphics::GraphicFormat::FMT_A8B8G8R8:
        case Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8:
        case Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16F:
        {
            return GL_RGBA;
        }
        case Enigma::Graphics::GraphicFormat::FMT_R8G8B8:
        {
            return GL_RGB;
        }
        case Enigma::Graphics::GraphicFormat::FMT_R32F:
        {
            return GL_RED;
        }
    }
    return GL_RGBA;
}

GLenum GraphicFormatToGLPixelType(const Enigma::Graphics::GraphicFormat& fmt)
{
    switch (fmt.fmt)
    {
        case Enigma::Graphics::GraphicFormat::FMT_A8B8G8R8:
        case Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8:
        case Enigma::Graphics::GraphicFormat::FMT_R8G8B8:
        {
            return GL_UNSIGNED_BYTE;
        }
        case Enigma::Graphics::GraphicFormat::FMT_A16B16G16R16F:
        case Enigma::Graphics::GraphicFormat::FMT_R32F:
        {
            return GL_FLOAT;
        }
    }
    return GL_UNSIGNED_BYTE;
}
