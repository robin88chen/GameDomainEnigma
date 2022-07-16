#include "GraphicAPIEgl.h"
#include "GraphicKernel/GraphicErrors.h"
#include "Platforms/PlatformLayer.h"
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
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension,
    unsigned buff_count, const std::vector<Graphics::GraphicFormat>& fmts)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::CreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
    const Graphics::GraphicFormat& fmt)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::ShareDepthStencilSurface(const std::string& depth_name,
    const Graphics::IDepthStencilSurfacePtr& from_depth)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::ClearSurface(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindBackSurface(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface)
{
    return ErrorCode::ok;
}

error GraphicAPIEgl::BindViewPort(const Graphics::TargetViewPort& vp)
{
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
