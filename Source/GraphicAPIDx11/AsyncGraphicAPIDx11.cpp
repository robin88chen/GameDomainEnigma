#include "AsyncGraphicAPIDx11.h"
#include "AsyncShaderCompilerDx11.h"
#include "GraphicAPIDx11.h"
#include "GraphicKernel/GraphicThread.h"
#include "GraphicKernel/GraphicErrors.h"

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;

AsyncGraphicAPIDx11::AsyncGraphicAPIDx11() : GraphicAPIDx11()
{
}

AsyncGraphicAPIDx11::~AsyncGraphicAPIDx11()
{
}

const std::unique_ptr<Enigma::Graphics::IShaderCompiler>& AsyncGraphicAPIDx11::GetShaderCompiler()
{
    assert(m_d3dDevice);
    if (!m_shaderCompiler)
    {
        m_shaderCompiler = std::make_unique<AsyncShaderCompilerDx11>(m_d3dDevice);
    }
    return m_shaderCompiler;
}

void AsyncGraphicAPIDx11::CreateDevice(const Graphics::DeviceRequiredBits& rqb, void* hwnd)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateDevice(rqb, hwnd); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CleanupDevice()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CleanupDevice(); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::BeginScene()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::BeginScene(); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::EndScene()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::EndScene(); return ErrorCode::ok; });
}
void AsyncGraphicAPIDx11::CreatePrimaryBackSurface(const std::string& back_name,
    const std::string& depth_name)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreatePrimaryBackSurface(back_name, depth_name); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreateVertexShader(const std::string& name)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateVertexShader(name); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreatePixelShader(const std::string& name)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreatePixelShader(name); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreateShaderProgram(const std::string& name, 
    const Graphics::IVertexShaderPtr& vtx_shader, const Graphics::IPixelShaderPtr& pix_shader, 
    const Graphics::IVertexDeclarationPtr& vtx_decl)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateShaderProgram(name, vtx_shader, pix_shader, vtx_decl); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreateVertexDeclaration(const std::string& name, const std::string& data_vertex_format, 
    const Graphics::IVertexShaderPtr& shader)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateVertexDeclaration(name, data_vertex_format, shader); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex,
    unsigned int sizeBuffer)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateVertexBuffer(buff_name, sizeofVertex, sizeBuffer); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::CreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::CreateIndexBuffer(buff_name, sizeBuffer); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::Clear(const Graphics::IBackSurfacePtr& back_surface,
    const Graphics::IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned int stencil_value)
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::Clear(back_surface, depth_surface, color, depth_value, stencil_value); return ErrorCode::ok; });
}

void AsyncGraphicAPIDx11::Flip()
{
    m_workerThread->PushTask([=]() -> error { GraphicAPIDx11::Flip(); return ErrorCode::ok; });
}