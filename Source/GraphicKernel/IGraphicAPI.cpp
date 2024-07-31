﻿#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "GraphicAssetStash.h"
#include "GraphicCommands.h"
#include "MathLib/ColorRGBA.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>
#include <memory>

using namespace Enigma::Graphics;

IGraphicAPI* IGraphicAPI::m_instance = nullptr;

IGraphicAPI::IGraphicAPI(AsyncType async) : m_wnd(nullptr), m_apiVersion(APIVersion::API_Unknown), m_boundTopology(PrimitiveTopology::Topology_Undefine)
{
    assert(!m_instance);
    m_instance = this;
    m_async = async;
    m_workerThread = nullptr;
    if (m_async == AsyncType::UseAsyncDevice) m_workerThread = new GraphicThread{};
    m_stash = new AssetStash{};
    subscribeHandlers();
}

IGraphicAPI::~IGraphicAPI()
{
    unsubscribeHandlers();
    m_instance = nullptr;
    if (m_workerThread) delete m_workerThread;
    if (m_stash) delete m_stash;
}

IGraphicAPI* IGraphicAPI::instance()
{
    assert(m_instance);
    return m_instance;
}

void IGraphicAPI::subscribeHandlers()
{
    m_createDevice =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->createDevice(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateDevice), m_createDevice);
    m_cleanDevice =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->cleanDevice(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CleanupDevice), m_cleanDevice);

    m_beginScene =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->beginScene(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BeginScene), m_beginScene);
    m_endScene =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->endScene(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::EndScene), m_endScene);

    m_drawPrimitive =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->drawPrimitive(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::DrawPrimitive), m_drawPrimitive);
    m_drawIndexedPrimitive =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->drawIndexedPrimitive(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::DrawIndexedPrimitive), m_drawIndexedPrimitive);

    m_clear =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->clear(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::ClearSurface), m_clear);
    m_flip =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->flip(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::FlipBackSurface), m_flip);

    m_createPrimarySurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->createPrimarySurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreatePrimarySurface), m_createPrimarySurface);
    m_createBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->createBackSurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateBacksurface), m_createBackSurface);
    m_doCreatingMultiBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingMultiBackSurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateMultiBacksurface), m_doCreatingMultiBackSurface);
    m_doCreatingDepthSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingDepthSurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateDepthStencilSurface), m_doCreatingDepthSurface);
    m_doSharingDepthSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoSharingDepthSurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::ShareDepthStencilSurface), m_doSharingDepthSurface);

    m_doCreatingVertexShader =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingVertexShader(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateVertexShader), m_doCreatingVertexShader);
    m_doCreatingPixelShader =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingPixelShader(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreatePixelShader), m_doCreatingPixelShader);
    m_doCreatingShaderProgram =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingShaderProgram(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateShaderProgram), m_doCreatingShaderProgram);
    m_doCreatingVertexDeclaration =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingVertexDeclaration(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateVertexDeclaration), m_doCreatingVertexDeclaration);

    m_doCreatingVertexBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingVertexBuffer(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateVertexBuffer), m_doCreatingVertexBuffer);
    m_doCreatingIndexBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingIndexBuffer(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateIndexBuffer), m_doCreatingIndexBuffer);

    m_doCreatingSamplerState =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingSamplerState(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateSamplerState), m_doCreatingSamplerState);
    m_doCreatingRasterizerState =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingRasterizerState(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateRasterizerState), m_doCreatingRasterizerState);
    m_doCreatingBlendState =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingBlendState(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateBlendState), m_doCreatingBlendState);
    m_doCreatingDepthStencilState =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoCreatingDepthStencilState(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::CreateDepthStencilState), m_doCreatingDepthStencilState);

    m_createTexture =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->createTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(CreateDeviceTexture), m_createTexture);
    m_createMultiTexture =
        std::make_shared<Frameworks::CommandSubscriber>([=](const Frameworks::ICommandPtr& c) { this->createMultiTexture(c); });
    Frameworks::CommandBus::subscribe(typeid(CreateDeviceMultiTexture), m_createMultiTexture);

    m_doBindingBackSurface =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingBackSurface(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BindBackSurface), m_doBindingBackSurface);
    m_doBindingViewPort =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingViewPort(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BindViewPort), m_doBindingViewPort);

    m_doBindingShaderProgram
        = std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingShaderProgram(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BindShaderProgram), m_doBindingShaderProgram);

    m_doBindingVertexBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingVertexBuffer(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BindVertexBuffer), m_doBindingVertexBuffer);
    m_doBindingIndexBuffer =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoBindingIndexBuffer(c); });
    Frameworks::CommandBus::subscribe(typeid(Graphics::BindIndexBuffer), m_doBindingIndexBuffer);
}

void IGraphicAPI::unsubscribeHandlers()
{
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateDevice), m_createDevice);
    m_createDevice = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CleanupDevice), m_cleanDevice);
    m_cleanDevice = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BeginScene), m_beginScene);
    m_beginScene = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::EndScene), m_endScene);
    m_endScene = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::DrawPrimitive), m_drawPrimitive);
    m_drawPrimitive = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::DrawIndexedPrimitive), m_drawIndexedPrimitive);
    m_drawIndexedPrimitive = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::ClearSurface), m_clear);
    m_clear = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::FlipBackSurface), m_flip);
    m_flip = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreatePrimarySurface), m_createPrimarySurface);
    m_createPrimarySurface = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateBacksurface), m_createBackSurface);
    m_createBackSurface = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateMultiBacksurface), m_doCreatingMultiBackSurface);
    m_doCreatingMultiBackSurface = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateDepthStencilSurface), m_doCreatingDepthSurface);
    m_doCreatingDepthSurface = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::ShareDepthStencilSurface), m_doSharingDepthSurface);
    m_doSharingDepthSurface = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateVertexShader), m_doCreatingVertexShader);
    m_doCreatingVertexShader = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreatePixelShader), m_doCreatingPixelShader);
    m_doCreatingPixelShader = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateShaderProgram), m_doCreatingShaderProgram);
    m_doCreatingShaderProgram = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateVertexDeclaration), m_doCreatingVertexDeclaration);
    m_doCreatingVertexDeclaration = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateVertexBuffer), m_doCreatingVertexBuffer);
    m_doCreatingVertexBuffer = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateIndexBuffer), m_doCreatingIndexBuffer);
    m_doCreatingIndexBuffer = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateSamplerState), m_doCreatingSamplerState);
    m_doCreatingSamplerState = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateRasterizerState), m_doCreatingRasterizerState);
    m_doCreatingRasterizerState = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateBlendState), m_doCreatingBlendState);
    m_doCreatingBlendState = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::CreateDepthStencilState), m_doCreatingDepthStencilState);
    m_doCreatingDepthStencilState = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(CreateDeviceTexture), m_createTexture);
    m_createTexture = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(CreateDeviceMultiTexture), m_createMultiTexture);
    m_createMultiTexture = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BindBackSurface), m_doBindingBackSurface);
    m_doBindingBackSurface = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BindViewPort), m_doBindingViewPort);
    m_doBindingViewPort = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BindShaderProgram), m_doBindingShaderProgram);
    m_doBindingShaderProgram = nullptr;

    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BindVertexBuffer), m_doBindingVertexBuffer);
    m_doBindingVertexBuffer = nullptr;
    Frameworks::CommandBus::unsubscribe(typeid(Graphics::BindIndexBuffer), m_doBindingIndexBuffer);
    m_doBindingIndexBuffer = nullptr;
}

void IGraphicAPI::beginScene()
{
    if (UseAsync())
    {
        asyncBeginDrawingScene();
    }
    else
    {
        beginDrawingScene();
    }
}

void IGraphicAPI::endScene()
{
    if (UseAsync())
    {
        asyncEndDrawingScene();
    }
    else
    {
        endDrawingScene();
    }
}

void IGraphicAPI::draw(unsigned vertexCount, unsigned vertexOffset)
{
    if (UseAsync())
    {
        asyncDrawPrimitive(vertexCount, vertexOffset);
    }
    else
    {
        drawPrimitive(vertexCount, vertexOffset);
    }
}

void IGraphicAPI::draw(unsigned indexCount, unsigned vertexCount, unsigned indexOffset, int baseVertexOffset)
{
    if (UseAsync())
    {
        asyncDrawIndexedPrimitive(indexCount, vertexCount, indexOffset, baseVertexOffset);
    }
    else
    {
        drawIndexedPrimitive(indexCount, vertexCount, indexOffset, baseVertexOffset);
    }
}

void IGraphicAPI::clear(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface,
    const MathLib::ColorRGBA& color, float depth_value, unsigned stencil_value)
{
    if (UseAsync())
    {
        AsyncClearSurface(back_surface, depth_surface, color, depth_value, stencil_value);
    }
    else
    {
        ClearSurface(back_surface, depth_surface, color, depth_value, stencil_value);
    }
}

void IGraphicAPI::flip()
{
    if (UseAsync())
    {
        asyncFlipBackSurface();
    }
    else
    {
        flipBackSurface();
    }
}

void IGraphicAPI::bind(const IBackSurfacePtr& back_surface, const IDepthStencilSurfacePtr& depth_surface)
{
    if (UseAsync())
    {
        AsyncBindBackSurface(back_surface, depth_surface);
    }
    else
    {
        BindBackSurface(back_surface, depth_surface);
    }
}

void IGraphicAPI::bind(const TargetViewPort& vp)
{
    if (UseAsync())
    {
        AsyncBindViewPort(vp);
    }
    else
    {
        bindViewPort(vp);
    }
}

void IGraphicAPI::bind(const IShaderProgramPtr& shader)
{
    if (UseAsync())
    {
        AsyncBindShaderProgram(shader);
    }
    else
    {
        BindShaderProgram(shader);
    }
}

void IGraphicAPI::bind(const IVertexBufferPtr& buffer, PrimitiveTopology pt)
{
    if (UseAsync())
    {
        AsyncBindVertexBuffer(buffer, pt);
    }
    else
    {
        BindVertexBuffer(buffer, pt);
    }
}

void IGraphicAPI::bind(const IIndexBufferPtr& buffer)
{
    if (UseAsync())
    {
        AsyncBindIndexBuffer(buffer);
    }
    else
    {
        BindIndexBuffer(buffer);
    }
}

void IGraphicAPI::createDevice(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateDevice, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        asyncCreateDevice(cmd->GetRequiredBits(), cmd->GetHwnd());
    }
    else
    {
        createDevice(cmd->GetRequiredBits(), cmd->GetHwnd());
    }
}

void IGraphicAPI::cleanDevice(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CleanupDevice, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        asyncCleanupDevice();
    }
    else
    {
        cleanupDevice();
    }
}

void IGraphicAPI::beginScene(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BeginScene, Frameworks::ICommand>(c);
    if (!cmd) return;
    beginScene();
}

void IGraphicAPI::endScene(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::EndScene, Frameworks::ICommand>(c);
    if (!cmd) return;
    endScene();
}

void IGraphicAPI::drawPrimitive(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::DrawPrimitive, Frameworks::ICommand>(c);
    if (!cmd) return;
    draw(cmd->GetVertexCount(), cmd->GetVertexOffset());
}

void IGraphicAPI::drawIndexedPrimitive(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::DrawIndexedPrimitive, Frameworks::ICommand>(c);
    if (!cmd) return;
    draw(cmd->GetIndexCount(), cmd->GetVertexCount(), cmd->GetIndexOffset(), cmd->GetBaseVertexOffset());
}
void IGraphicAPI::clear(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::ClearSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    clear(cmd->GetBackSurface(), cmd->GetDepthSurface(), cmd->GetColor(),
        cmd->GetDepthValue(), cmd->GetStencilValue());
}

void IGraphicAPI::flip(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::FlipBackSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    flip();
}

void IGraphicAPI::createPrimarySurface(const Frameworks::ICommandPtr& c)
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

void IGraphicAPI::createBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateBacksurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateBackSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().format());
    }
    else
    {
        CreateBackSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().format());
    }
}

void IGraphicAPI::DoCreatingMultiBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateMultiBacksurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateBackSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().surfaceCount(), cmd->specification().formats());
    }
    else
    {
        CreateBackSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().surfaceCount(), cmd->specification().formats());
    }
}

void IGraphicAPI::DoCreatingDepthSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateDepthStencilSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateDepthStencilSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().format());
    }
    else
    {
        CreateDepthStencilSurface(cmd->specification().name(), cmd->specification().dimension(), cmd->specification().format());
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
        AsyncCreateVertexShader(cmd->getName());
    }
    else
    {
        CreateVertexShader(cmd->getName());
    }
}

void IGraphicAPI::DoCreatingPixelShader(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreatePixelShader, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreatePixelShader(cmd->getName());
    }
    else
    {
        CreatePixelShader(cmd->getName());
    }
}

void IGraphicAPI::DoCreatingShaderProgram(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateShaderProgram, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateShaderProgram(cmd->getName(), cmd->GetVertexShader(), cmd->GetPixelShader(), cmd->GetVertexDeclaration());
    }
    else
    {
        CreateShaderProgram(cmd->getName(), cmd->GetVertexShader(), cmd->GetPixelShader(), cmd->GetVertexDeclaration());
    }
}

void IGraphicAPI::DoCreatingVertexDeclaration(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateVertexDeclaration, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateVertexDeclaration(cmd->getName(), cmd->GetDataVertexFormat(), cmd->GetShader());
    }
    else
    {
        CreateVertexDeclaration(cmd->getName(), cmd->GetDataVertexFormat(), cmd->GetShader());
    }
}

void IGraphicAPI::DoCreatingVertexBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateVertexBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateVertexBuffer(cmd->getName(), cmd->GetSizeofVertex(), cmd->GetSizeBuffer());
    }
    else
    {
        CreateVertexBuffer(cmd->getName(), cmd->GetSizeofVertex(), cmd->GetSizeBuffer());
    }
}

void IGraphicAPI::DoCreatingIndexBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateIndexBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateIndexBuffer(cmd->getName(), cmd->GetSizeBuffer());
    }
    else
    {
        CreateIndexBuffer(cmd->getName(), cmd->GetSizeBuffer());
    }
}

void IGraphicAPI::DoCreatingSamplerState(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateSamplerState, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateSamplerState(cmd->getName(), cmd->GetData());
    }
    else
    {
        CreateSamplerState(cmd->getName(), cmd->GetData());
    }
}

void IGraphicAPI::DoCreatingRasterizerState(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateRasterizerState, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateRasterizerState(cmd->getName(), cmd->GetData());
    }
    else
    {
        CreateRasterizerState(cmd->getName(), cmd->GetData());
    }
}

void IGraphicAPI::DoCreatingBlendState(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateBlendState, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateAlphaBlendState(cmd->getName(), cmd->GetData());
    }
    else
    {
        CreateAlphaBlendState(cmd->getName(), cmd->GetData());
    }
}

void IGraphicAPI::DoCreatingDepthStencilState(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::CreateDepthStencilState, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        AsyncCreateDepthStencilState(cmd->getName(), cmd->GetData());
    }
    else
    {
        CreateDepthStencilState(cmd->getName(), cmd->GetData());
    }
}

void IGraphicAPI::createTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<CreateDeviceTexture, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        asyncCreateTexture(cmd->name());
    }
    else
    {
        createTexture(cmd->name());
    }
}

void IGraphicAPI::createMultiTexture(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<CreateDeviceMultiTexture, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (UseAsync())
    {
        asyncCreateMultiTexture(cmd->name());
    }
    else
    {
        createMultiTexture(cmd->name());
    }
}

void IGraphicAPI::DoBindingBackSurface(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindBackSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    bind(cmd->GetBackSurface(), cmd->GetDepthSurface());
}

void IGraphicAPI::DoBindingViewPort(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindViewPort, Frameworks::ICommand>(c);
    if (!cmd) return;
    bind(cmd->getViewPort());
}

void IGraphicAPI::DoBindingShaderProgram(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindShaderProgram, Frameworks::ICommand>(c);
    if (!cmd) return;
    bind(cmd->GetShader());
}

void IGraphicAPI::DoBindingVertexBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindVertexBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    bind(cmd->GetBuffer(), cmd->GetTopology());
}

void IGraphicAPI::DoBindingIndexBuffer(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<Graphics::BindIndexBuffer, Frameworks::ICommand>(c);
    if (!cmd) return;
    bind(cmd->GetBuffer());
}

future_error IGraphicAPI::asyncCreateDevice(const DeviceRequiredBits& rqb, void* hwnd)
{
    return m_workerThread->PushTask([=]() -> error { return this->createDevice(rqb, hwnd); });
}

future_error IGraphicAPI::asyncCleanupDevice()
{
    return m_workerThread->PushTask([=]() -> error { return this->cleanupDevice(); });
}

future_error IGraphicAPI::asyncBeginDrawingScene()
{
    return m_workerThread->PushTask([=]() -> error { return this->beginDrawingScene(); });
}

future_error IGraphicAPI::asyncEndDrawingScene()
{
    return m_workerThread->PushTask([=]() -> error { return this->endDrawingScene(); });
}

future_error IGraphicAPI::asyncDrawPrimitive(unsigned vertexCount, unsigned vertexOffset)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->drawPrimitive(vertexCount, vertexOffset); });
}

future_error IGraphicAPI::asyncDrawIndexedPrimitive(unsigned indexCount, unsigned vertexCount, unsigned indexOffset,
    int baseVertexOffset)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->drawIndexedPrimitive(indexCount, vertexCount, indexOffset, baseVertexOffset); });
}

future_error IGraphicAPI::asyncFlipBackSurface()
{
    return m_workerThread->PushTask([=]() -> error { return this->flipBackSurface(); });
}

future_error IGraphicAPI::AsyncCreatePrimaryBackSurface(const std::string& back_name, const std::string& depth_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreatePrimaryBackSurface(back_name, depth_name); });
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    const GraphicFormat& fmt)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, fmt); });
}

future_error IGraphicAPI::AsyncCreateBackSurface(const std::string& back_name, const MathLib::Dimension<unsigned>& dimension,
    unsigned buff_count, const std::vector<GraphicFormat>& fmts)
{
    return m_workerThread->PushTask([=]() -> error
        { return this->CreateBackSurface(back_name, dimension, buff_count, fmts); });
}

future_error IGraphicAPI::AsyncCreateDepthStencilSurface(const std::string& depth_name, const MathLib::Dimension<unsigned>& dimension,
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
        { return this->bindViewPort(vp); });
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

future_error IGraphicAPI::AsyncCreateVertexBuffer(const std::string& buff_name, unsigned int sizeofVertex, unsigned int sizeBuffer)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateVertexBuffer(buff_name, sizeofVertex, sizeBuffer); });
}

future_error IGraphicAPI::AsyncCreateIndexBuffer(const std::string& buff_name, unsigned int sizeBuffer)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateIndexBuffer(buff_name, sizeBuffer); });
}

future_error IGraphicAPI::AsyncCreateSamplerState(const std::string& name, const IDeviceSamplerState::SamplerStateData& data)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateSamplerState(name, data); });
}

future_error IGraphicAPI::AsyncCreateRasterizerState(const std::string& name, const IDeviceRasterizerState::RasterizerStateData& data)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateRasterizerState(name, data); });
}

future_error IGraphicAPI::AsyncCreateAlphaBlendState(const std::string& name, const IDeviceAlphaBlendState::BlendStateData& data)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateAlphaBlendState(name, data); });
}

future_error IGraphicAPI::AsyncCreateDepthStencilState(const std::string& name, const IDeviceDepthStencilState::DepthStencilData& data)
{
    return m_workerThread->PushTask([=]() -> error { return this->CreateDepthStencilState(name, data); });
}

future_error IGraphicAPI::asyncCreateTexture(const std::string& tex_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->createTexture(tex_name); });
}

future_error IGraphicAPI::asyncCreateMultiTexture(const std::string& tex_name)
{
    return m_workerThread->PushTask([=]() -> error { return this->createMultiTexture(tex_name); });
}

future_error IGraphicAPI::AsyncBindVertexBuffer(const IVertexBufferPtr& buffer, PrimitiveTopology pt)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindVertexBuffer(buffer, pt); });
}

future_error IGraphicAPI::AsyncBindIndexBuffer(const IIndexBufferPtr& buffer)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindIndexBuffer(buffer); });
}

future_error IGraphicAPI::AsyncBindShaderProgram(const IShaderProgramPtr& shader)
{
    return m_workerThread->PushTask([=]() -> error { return this->BindShaderProgram(shader); });
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

bool IGraphicAPI::IsValidGraphicThread(const std::thread::id& id)
{
    if (!m_workerThread) return true;
    return id == m_workerThread->GetThreadId();
}
