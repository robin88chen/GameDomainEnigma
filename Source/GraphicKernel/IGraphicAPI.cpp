#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "GraphicAssetStash.h"
#include "IVertexShader.h"
#include "IVertexDeclaration.h"
#include "GraphicCommands.h"
#include "MathLib/ColorRGBA.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>
#include <memory>

using namespace Enigma::Graphics;

IGraphicAPI* IGraphicAPI::m_instance = nullptr;

IGraphicAPI::IGraphicAPI(AsyncType async)
{
    assert(!m_instance);
    m_instance = this;
    m_async = async;
    m_workerThread = new GraphicThread{};
    m_stash = new AssetStash{};
    SubscribeHandlers();
}

IGraphicAPI::~IGraphicAPI()
{
    UnsubscribeHandlers();
    m_instance = nullptr;
    if (m_workerThread) delete m_workerThread;
    if (m_stash) delete m_stash;
}

IGraphicAPI* IGraphicAPI::Instance()
{
    assert(m_instance);
    return m_instance;
}

void IGraphicAPI::SubscribeHandlers()
{
    m_doCreatingDevice =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingDevice(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateDevice), m_doCreatingDevice);
    m_doCleaningDevice =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCleaningDevice(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CleanupDevice), m_doCleaningDevice);

    m_doBeginningScene =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBeginningScene(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::BeginScene), m_doBeginningScene);
    m_doEndingScene =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoEndingScene(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::EndScene), m_doEndingScene);

    m_doDrawingPrimitive =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoDrawingPrimitive(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::DrawPrimitive), m_doDrawingPrimitive);
    m_doDrawingIndexedPrimitive =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoDrawingIndexedPrimitive(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::DrawIndexedPrimitive), m_doDrawingIndexedPrimitive);

	m_doClearing =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoClearing(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::ClearSurface), m_doClearing);
    m_doFlipping =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoFlipping(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::FlipBackSurface), m_doFlipping);

	m_doCreatingPrimarySurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingPrimarySurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreatePrimarySurface), m_doCreatingPrimarySurface);
    m_doCreatingBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingBackSurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateBacksurface), m_doCreatingBackSurface);
    m_doCreatingMultiBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingMultiBackSurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateMultiBacksurface), m_doCreatingMultiBackSurface);
    m_doCreatingDepthSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingDepthSurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateDepthStencilSurface), m_doCreatingDepthSurface);
    m_doSharingDepthSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoSharingDepthSurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::ShareDepthStencilSurface), m_doSharingDepthSurface);

    m_doCreatingVertexShader =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingVertexShader(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateVertexShader), m_doCreatingVertexShader);
    m_doCreatingPixelShader =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingPixelShader(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreatePixelShader), m_doCreatingPixelShader);
    m_doCreatingShaderProgram =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingShaderProgram(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::CreateShaderProgram), m_doCreatingShaderProgram);

    m_doBindingBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingBackSurface(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::BindBackSurface), m_doBindingBackSurface);
    m_doBindingViewPort =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingViewPort(c); });
    Frameworks::CommandBus::Subscribe(typeid(Graphics::BindViewPort), m_doBindingViewPort);
}

void IGraphicAPI::UnsubscribeHandlers()
{
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateDevice), m_doCreatingDevice);
    m_doCreatingDevice = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CleanupDevice), m_doCleaningDevice);
    m_doCleaningDevice = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::BeginScene), m_doBeginningScene);
    m_doBeginningScene = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::EndScene), m_doEndingScene);
    m_doEndingScene = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::DrawPrimitive), m_doDrawingPrimitive);
    m_doDrawingPrimitive = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::DrawIndexedPrimitive), m_doDrawingIndexedPrimitive);
    m_doDrawingIndexedPrimitive = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::ClearSurface), m_doClearing);
    m_doClearing = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::FlipBackSurface), m_doFlipping);
    m_doFlipping = nullptr;

	Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreatePrimarySurface), m_doCreatingPrimarySurface);
    m_doCreatingPrimarySurface = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateBacksurface), m_doCreatingBackSurface);
    m_doCreatingBackSurface = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateMultiBacksurface), m_doCreatingMultiBackSurface);
    m_doCreatingMultiBackSurface = nullptr;
        
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateDepthStencilSurface), m_doCreatingDepthSurface);
    m_doCreatingDepthSurface = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::ShareDepthStencilSurface), m_doSharingDepthSurface);
    m_doSharingDepthSurface = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateVertexShader), m_doCreatingVertexShader);
    m_doCreatingVertexShader = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreatePixelShader), m_doCreatingPixelShader);
    m_doCreatingPixelShader = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::CreateShaderProgram), m_doCreatingShaderProgram);
    m_doCreatingShaderProgram = nullptr;

    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::BindBackSurface), m_doBindingBackSurface);
    m_doBindingBackSurface = nullptr;
    Frameworks::CommandBus::Unsubscribe(typeid(Graphics::BindViewPort), m_doBindingViewPort);
    m_doBindingViewPort = nullptr;
}

void IGraphicAPI::DoCreatingDevice(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateDevice, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateDevice(cmd->GetRequiredBits(), cmd->GetHwnd());
    }
    else
    {
        CreateDevice(cmd->GetRequiredBits(), cmd->GetHwnd());
    }
}

void IGraphicAPI::DoCleaningDevice(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CleanupDevice, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCleanupDevice();
    }
    else
    {
        CleanupDevice();
    }
}

void IGraphicAPI::DoBeginningScene(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BeginScene, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncBeginScene();
    }
    else
    {
        BeginScene();
    }
}

void IGraphicAPI::DoEndingScene(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::EndScene, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncEndScene();
    }
    else
    {
        EndScene();
    }
}

void IGraphicAPI::DoDrawingPrimitive(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::DrawPrimitive, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncDrawPrimitive(cmd->GetVertexCount(), cmd->GetVertexOffset());
    }
    else
    {
        DrawPrimitive(cmd->GetVertexCount(), cmd->GetVertexOffset());
    }
}

void IGraphicAPI::DoDrawingIndexedPrimitive(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::DrawIndexedPrimitive, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncDrawIndexedPrimitive(cmd->GetIndexCount(), cmd->GetVertexCount(), cmd->GetIndexOffset(), cmd->GetBaseVertexOffset());
    }
    else
    {
        DrawIndexedPrimitive(cmd->GetIndexCount(), cmd->GetVertexCount(), cmd->GetIndexOffset(), cmd->GetBaseVertexOffset());
    }
}
void IGraphicAPI::DoClearing(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::ClearSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncClearSurface(cmd->GetBackSurface(), cmd->GetDepthSurface(), cmd->GetColor(), 
            cmd->GetDepthValue(), cmd->GetStencilValue());
    }
    else
    {
        ClearSurface(cmd->GetBackSurface(), cmd->GetDepthSurface(), cmd->GetColor(),
            cmd->GetDepthValue(), cmd->GetStencilValue());
    }
}

void IGraphicAPI::DoFlipping(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::FlipBackSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncFlip();
    }
    else
    {
        Flip();
    }
}

void IGraphicAPI::DoCreatingPrimarySurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreatePrimarySurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreatePrimaryBackSurface(cmd->GetBacksurfaceName(), cmd->GetDepthsurfaceName());
    }
    else
    {
        CreatePrimaryBackSurface(cmd->GetBacksurfaceName(), cmd->GetDepthsurfaceName());
    }
}

void IGraphicAPI::DoCreatingBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateBacksurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateBackSurface(cmd->GetBacksurfaceName(), cmd->GetDimension(), cmd->GetFormat());
    }
    else
    {
        CreateBackSurface(cmd->GetBacksurfaceName(), cmd->GetDimension(), cmd->GetFormat());
    }
}

void IGraphicAPI::DoCreatingMultiBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateMultiBacksurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateBackSurface(cmd->GetBacksurfaceName(), cmd->GetDimension(), cmd->GetSurfaceCount(), cmd->GetFormats());
    }
    else
    {
        CreateBackSurface(cmd->GetBacksurfaceName(), cmd->GetDimension(), cmd->GetSurfaceCount(), cmd->GetFormats());
    }
}

void IGraphicAPI::DoCreatingDepthSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateDepthStencilSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateDepthStencilSurface(cmd->GetDepthStencilSurfaceName(), cmd->GetDimension(), cmd->GetFormat());
    }
    else
    {
        CreateDepthStencilSurface(cmd->GetDepthStencilSurfaceName(), cmd->GetDimension(), cmd->GetFormat());
    }
}

void IGraphicAPI::DoSharingDepthSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::ShareDepthStencilSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncShareDepthStencilSurface(cmd->GetDepthStencilSurfaceName(), cmd->GetSourceDepth());
    }
    else
    {
        ShareDepthStencilSurface(cmd->GetDepthStencilSurfaceName(), cmd->GetSourceDepth());
    }
}

void IGraphicAPI::DoCreatingVertexShader(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateVertexShader, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateVertexShader(cmd->GetName());
    }
    else
    {
        CreateVertexShader(cmd->GetName());
    }
}

void IGraphicAPI::DoCreatingPixelShader(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreatePixelShader, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreatePixelShader(cmd->GetName());
    }
    else
    {
        CreatePixelShader(cmd->GetName());
    }
}

void IGraphicAPI::DoCreatingShaderProgram(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateShaderProgram, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateShaderProgram(cmd->GetName(), cmd->GetVertexShader(), cmd->GetPixelShader(), cmd->GetVertexDeclaration());
    }
    else
    {
        CreateShaderProgram(cmd->GetName(), cmd->GetVertexShader(), cmd->GetPixelShader(), cmd->GetVertexDeclaration());
    }
}

void IGraphicAPI::DoBindingBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindBackSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncBindBackSurface(cmd->GetBackSurface(), cmd->GetDepthSurface());
    }
    else
    {
        BindBackSurface(cmd->GetBackSurface(), cmd->GetDepthSurface());
    }
}

void IGraphicAPI::DoBindingViewPort(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindViewPort, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncBindViewPort(cmd->GetViewPort());
    }
    else
    {
        BindViewPort(cmd->GetViewPort());
    }
}

future_error IGraphicAPI::AsyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateDevice(rqb, hwnd); });
}

future_error IGraphicAPI::AsyncCleanupDevice()
{
    return m_workerThread->PushTask([=]() -> error { return this->CleanupDevice(); });
}

future_error IGraphicAPI::AsyncBeginScene()
{
    return m_workerThread->PushTask([=]() -> error { return this->BeginScene(); });
}

future_error IGraphicAPI::AsyncEndScene()
{
    return m_workerThread->PushTask([=]() -> error { return this->EndScene(); });
}

future_error IGraphicAPI::AsyncDrawPrimitive(unsigned vertexCount, unsigned vertexOffset)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->DrawPrimitive(vertexCount, vertexOffset); });
}

future_error IGraphicAPI::AsyncDrawIndexedPrimitive(unsigned indexCount, unsigned vertexCount, unsigned indexOffset,
    int baseVertexOffset)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->DrawIndexedPrimitive(indexCount, vertexCount, indexOffset, baseVertexOffset); });
}

future_error IGraphicAPI::AsyncFlip()
{
    return m_workerThread->PushTask([=]() -> error { return this->Flip(); });
}

future_error IGraphicAPI::AsyncCreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreatePrimaryBackSurface(back_name, depth_name); });
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    const GraphicFormat& fmt)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, fmt); });
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension& dimension, 
    unsigned buff_count, const std::vector<GraphicFormat>& fmts)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, buff_count, fmts); });
}

future_error IGraphicAPI::AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension& dimension,
    const GraphicFormat& fmt)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateDepthStencilSurface(depth_name, dimension, fmt); });
}

future_error IGraphicAPI::AsyncShareDepthStencilSurface(const std::string& depth_name, 
    const IDepthStencilSurfacePtr& from_depth)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ShareDepthStencilSurface(depth_name, from_depth); });
}

future_error IGraphicAPI::AsyncClearSurface(const IBackSurfacePtr& back_surface,
    const IDepthStencilSurfacePtr& depth_surface, const MathLib::ColorRGBA& color, float depth_value,
    unsigned stencil_value)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->ClearSurface(back_surface, depth_surface, color, depth_value, stencil_value); });
}

future_error IGraphicAPI::AsyncBindBackSurface(const IBackSurfacePtr& back_surface,
    const IDepthStencilSurfacePtr& depth_surface)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->BindBackSurface(back_surface, depth_surface); });
}

future_error IGraphicAPI::AsyncBindViewPort(const TargetViewPort& vp)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->BindViewPort(vp); });
}

future_error IGraphicAPI::AsyncCreateVertexShader(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateVertexShader(name); });
}

future_error IGraphicAPI::AsyncCreatePixelShader(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreatePixelShader(name); });
}

future_error IGraphicAPI::AsyncCreateShaderProgram(const std::string& name,
    const IVertexShaderPtr& vtx_shader, const IPixelShaderPtr& pix_shader, const IVertexDeclarationPtr& vtx_decl)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateShaderProgram(name, vtx_shader, pix_shader, vtx_decl); });
}

future_error IGraphicAPI::AsyncCreateVertexDeclaration(const std::string& name,
    const std::string& data_vertex_format, const IVertexShaderPtr& shader)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateVertexDeclaration(name, data_vertex_format, shader); });
}

future_error IGraphicAPI::AsyncCreateVertexBuffer(const std::string& buff_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateVertexBuffer(buff_name); });
}

future_error IGraphicAPI::AsyncCreateIndexBuffer(const std::string& buff_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateIndexBuffer(buff_name); });
}

future_error IGraphicAPI::AsyncCreateSamplerState(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateSamplerState(name); });
}

future_error IGraphicAPI::AsyncCreateRasterizerState(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateRasterizerState(name); });
}

future_error IGraphicAPI::AsyncCreateAlphaBlendState(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateDepthStencilState(name); });
}

future_error IGraphicAPI::AsyncCreateDepthStencilState(const std::string& name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateDepthStencilState(name); });
}

future_error IGraphicAPI::AsyncCreateTexture(const std::string& tex_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateTexture(tex_name); });
}

future_error IGraphicAPI::AsyncCreateMultiTexture(const std::string& tex_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateMultiTexture(tex_name); });
}

future_error IGraphicAPI::AsyncBindVertexDeclaration(const IVertexDeclarationPtr& vertexDecl)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindVertexDeclaration(vertexDecl); });
}

future_error IGraphicAPI::AsyncBindVertexShader(const IVertexShaderPtr& shader)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindVertexShader(shader); });
}

future_error IGraphicAPI::AsyncBindPixelShader(const IPixelShaderPtr& shader)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindPixelShader(shader); });
}

future_error IGraphicAPI::AsyncBindVertexBuffer(const IVertexBufferPtr& buffer, PrimitiveTopology pt)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindVertexBuffer(buffer, pt); });
}

future_error IGraphicAPI::AsyncBindIndexBuffer(const IIndexBufferPtr& buffer)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindIndexBuffer(buffer); });
}

void IGraphicAPI::TerminateGraphicThread()
{
    if (m_workerThread)
    {
        m_workerThread->Terminate();
    }
}

GraphicThread* IGraphicAPI::GetGraphicThread()
{
    assert(m_workerThread);
    return m_workerThread;
}

