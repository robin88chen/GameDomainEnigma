#include "EffectMaterialManagerTest.h"
#include "GameEngine/EffectCompiler.h"
#include "GameEngine/EffectCompilingPolicies.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/RendererEvents.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererCommands.h"
#include "GameEngine/EffectEvents.h"
#include "GraphicKernel/GraphicEvents.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Controllers/InstallingPolicies.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Platforms/MemoryAllocMacro.h"
#include "BufferBuilder.h"
#include "TextureSamplerBuilder.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::Controllers;
using namespace Enigma::MathLib;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string VertexBufferName = "vtx_buffer";
std::string IndexBufferName = "idx_buffer";
std::string TextureName = "earth";
std::string MaterialName = "test_material";
std::string ShaderProgramName = "shader_program";
std::string VertexDeclName = "vtx_layout";
std::string PassTopLeftName = "pass_top_left";
std::string PassTopRightName = "pass_top_right";
std::string PassBottomLeftName = "pass_bottom_left";
std::string PassBottomRightName = "pass_bottom_right";

struct VtxData
{
    Vector3 pos;
    Vector2 coord;
};
static VtxData vtx_pos[] =
{
    { Vector3(-0.25f, 0.25f, 0.5f), Vector2(0.0f, 0.0f)},
    { Vector3(0.25f, 0.25f, 0.5f), Vector2(1.0f, 0.0f) },
    { Vector3(-0.25f, -0.25f, 0.5f), Vector2(0.0f, 1.0f) },
    { Vector3(0.25f, -0.25f, 0.5f), Vector2(1.0f, 1.0f) },
};
static unsigned int vtx_idx[] =
{
    0, 1, 2, 1, 3, 2,
};

static std::string vs_code_11 =
"float offset_x;\n"
"float offset_y;\n"
"struct VS_INPUT \n"
"{\n"
"   float3 Pos : POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"};\n"
"struct VS_OUTPUT\n"
"{\n"
"   float4 Pos : SV_POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"   float4 DiffCol : COLOR0; \n"
"};\n"
"VS_OUTPUT vs_main(const VS_INPUT v)\n"
"{\n"
"   VS_OUTPUT o = (VS_OUTPUT)0;"
"   o.Pos.xyz = v.Pos + float3(offset_x, offset_y, 0.0f);\n"
" o.Pos.w = 1.0f;\n"
"   o.Coord = v.Coord;\n"
"   o.DiffCol = float4(0.5f + v.Pos.x + v.Pos.y, 0.5f + v.Pos.x - v.Pos.y, 0.5f + v.Pos.y - v.Pos.x, 1.0f); \n"
"   return o;\n"
"};\n"
"";

static std::string ps_code_11 =
"//semantic anim_time ANIM_TIMER\n\r"
"//semantic anim_scale ANIM_SCALE\n\r"
"Texture2D DiffuseTexture : DiffuseMap;\n"
"SamplerState samLinear\n"
"{\n"
"   Filter = MIN_MAG_MIP_LINEAR;\n"
"   AddressU = Wrap;\n"
"   AddressV = Wrap;\n"
"};\n"
"float anim_time : ANIM_TIMER;\n"
"float anim_scale : ANIM_SCALE; \n"
"struct PS_INPUT\n"
"{\n"
"   float4 Pos : SV_POSITION;\n"
"   float2 Coord : TEXCOORD0;\n"
"   float4 DiffCol : COLOR0; \n"
"};\n"
"float4 ps_main(PS_INPUT input) : SV_TARGET\n"
"{\n"
" float2 co = float2(input.Coord.x + anim_time, input.Coord.y);\n"
"   float4 outputColor = (0.5f + cos(anim_time * anim_scale) / 2.0f) * input.DiffCol;\n"
"   outputColor = DiffuseTexture.Sample(samLinear, co) * outputColor;\n"
"   outputColor.a = 1;\n"
"   return outputColor;\n"
"};\n";

static const std::string vs_code_egl =
"#version 300 es\n"
"precision mediump float;\n"
"uniform float offset_x;\n"
"uniform float offset_y;\n"
"layout(location = 0) in vec3 pos;\n"
"layout(location = 1) in vec2 texco;\n"
"out vec2 vTexCo;\n"
"out vec4 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(pos, 1.0) + vec4(offset_x, offset_y, 0.0f, 0.0f);\n"
"    vColor = vec4(0.5f + pos.x + pos.y, 0.5f + pos.x - pos.y, 0.5f + pos.y - pos.x, 1.0f);\n"
"    vTexCo = texco;\n"
"}\n";

static const std::string ps_code_egl =
"#version 300 es\n"
"//semantic anim_time ANIM_TIMER\n"
"//semantic anim_scale ANIM_SCALE\n"
"precision mediump float;\n"
"//sampler_state samLinear DiffuseTexture\n"
"uniform sampler2D DiffuseTexture;\n"
"uniform float anim_time;\n"
"uniform float anim_scale;\n"
"in vec4 vColor;\n"
"in vec2 vTexCo;\n"
"out vec4 outColor;\n"
"void main() {\n"
"    vec2 co = vec2(vTexCo.x + anim_time, vTexCo.y);\n"
"    outColor = (0.5f + cos(anim_time * anim_scale)/2.0f) * vColor;\n"
"    outColor = texture(DiffuseTexture, co) * outColor;\n"
"}\n";

EffectMaterialManagerTest::EffectMaterialManagerTest(const std::string app_name) : AppDelegate(app_name)
{
    m_material = nullptr;
}

EffectMaterialManagerTest::~EffectMaterialManagerTest()
{

}

void EffectMaterialManagerTest::InitializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::Instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../../Media/";
        FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
    }
#endif
}

void EffectMaterialManagerTest::InstallEngine()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectCompileFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectCompileFailed(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompileFailed), m_onEffectCompileFailed);

    m_onVertexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnVertexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onIndexBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnIndexBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);

    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));
    m_rendererManager = ServiceManager::GetSystemServiceAs<RendererManager>();
    m_materialManager = ServiceManager::GetSystemServiceAs<EffectMaterialManager>();
    m_materialManager->CompileEffectMaterial(MakeEffectCompilingPolicy());

    byte_buffer points = make_data_buffer((unsigned char*)vtx_pos, sizeof(vtx_pos));
    uint_buffer indices = make_data_buffer(vtx_idx, 6);
    m_bufferBuilder = menew BufferBuilder();
    m_bufferBuilder->BuildVertexBuffer(VertexBufferName, sizeof(VtxData), points);
    m_bufferBuilder->BuildIndexBuffer(IndexBufferName, indices);

    m_textureBuilder = menew TextureSamplerBuilder();
    m_textureBuilder->BuildTexture(TextureName, "earth.png", MediaPathName);

    m_timer = menew Timer;
    m_timer->Reset();
    m_tick = 0.0f;
}

void EffectMaterialManagerTest::ShutdownEngine()
{
    delete m_bufferBuilder;
    m_bufferBuilder = nullptr;
    delete m_textureBuilder;
    m_textureBuilder = nullptr;
    delete m_timer;
    m_timer = nullptr;

    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::Unsubscribe(typeid(EffectMaterialCompileFailed), m_onEffectCompileFailed);
    m_onEffectCompileFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(VertexBufferBuilt), m_onVertexBufferBuilt);
    m_onVertexBufferBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(IndexBufferBuilt), m_onIndexBufferBuilt);
    m_onIndexBufferBuilt = nullptr;

    EventPublisher::Unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;

    m_renderTarget = nullptr;
    m_vtxBuffer = nullptr;
    m_idxBuffer = nullptr;
    m_texture = nullptr;

    m_material = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void EffectMaterialManagerTest::FrameUpdate()
{
    AppDelegate::FrameUpdate();
    if (m_timer)
    {
        m_timer->Update();
        m_tick += m_timer->GetElapseTime();
    }
    if (m_material)
    {
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassTopRightName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassTopRightName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(3.0f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassBottomLeftName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(-m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassBottomLeftName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(1.5f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassBottomRightName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassBottomRightName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(3.0f);
        }
        if (auto var_timer = m_material->GetEffectVariableInPassBySemantic(PassTopLeftName, "ANIM_TIMER"))
        {
            var_timer.value().get().AssignValue(-m_tick);
        }
        if (auto var_scale = m_material->GetEffectVariableInPassBySemantic(PassTopLeftName, "ANIM_SCALE"))
        {
            var_scale.value().get().AssignValue(1.5f);
        }
    }
}

void EffectMaterialManagerTest::RenderFrame()
{
    if ((!m_material) || (!m_vtxBuffer) || (!m_idxBuffer) || (!m_renderTarget)) return;
    m_renderTarget->Bind();
    m_renderTarget->BindViewPort();
    IGraphicAPI::Instance()->Bind(m_vtxBuffer, PrimitiveTopology::Topology_TriangleList);
    IGraphicAPI::Instance()->Bind(m_idxBuffer);
    m_renderTarget->Clear();
    IGraphicAPI::Instance()->BeginScene();
    m_material->SelectRendererTechnique("technique");
    //m_material->SelectVisualTechnique("slash");
    m_material->SelectVisualTechnique("backslash");
    m_material->ApplyFirstPass();
    IGraphicAPI::Instance()->Draw(6, 4, 0, 0);
    m_material->ApplyNextPass();
    IGraphicAPI::Instance()->Draw(6, 4, 0, 0);
    IGraphicAPI::Instance()->EndScene();
    m_renderTarget->Flip();
}

EffectCompilingPolicy EffectMaterialManagerTest::MakeEffectCompilingPolicy()
{
    EffectCompilingPolicy policy;
    policy.m_name = MaterialName;
#if TARGET_PLATFORM == PLATFORM_WIN32
    ShaderProgramPolicy program{
        ShaderProgramName, "vtx_shader", "xyz_tex1(2)", vs_code_11, "vs_4_0", "vs_main", VertexDeclName, "pix_shader", ps_code_11, "ps_4_0", "ps_main" };
#else
    ShaderProgramPolicy program{
        ShaderProgramName, "vtx_shader", "xyz_tex1(2)", vs_code_egl, "vs_4_0", "vs_main",VertexDeclName, "pix_shader", ps_code_egl, "ps_4_0", "ps_main" };
#endif
    IDeviceSamplerState::SamplerStateData samp_data;
    samp_data.m_addressModeU = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    samp_data.m_addressModeV = IDeviceSamplerState::SamplerStateData::AddressMode::Wrap;
    EffectPassProfile pass_profile{ PassTopRightName, program,
        {{"samLinear", samp_data, "samLinear"}},
        {"raster"}, {"blender"}, {"depth"} };
    EffectPassProfile pass_profile2 = pass_profile;
    pass_profile2.m_name = PassBottomLeftName;
    EffectTechniqueProfile technique_profile{ "technique/slash", {pass_profile, pass_profile2} };
    EffectTechniqueProfile technique_profile2 = technique_profile;
    technique_profile2.m_name = "technique/backslash";
    technique_profile2.m_passes[0].m_name = PassBottomRightName;
    technique_profile2.m_passes[1].m_name = PassTopLeftName;
    policy.m_techniques = { technique_profile, technique_profile2 };
    return policy;
}

void EffectMaterialManagerTest::OnRenderTargetCreated(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<PrimaryRenderTargetCreated> ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = m_rendererManager->GetRenderTarget(ev->GetRenderTargetName());
}

void EffectMaterialManagerTest::OnEffectMaterialCompiled(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<EffectMaterialCompiled> ev = std::dynamic_pointer_cast<EffectMaterialCompiled, IEvent>(e);
    if ((!ev) || (!ev->HasEffect())) return;
    if (ev->GetFilename() != MaterialName) return;
    m_material = ev->GetEffect();
    BuildVariables();
}

void EffectMaterialManagerTest::OnEffectCompileFailed(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<EffectMaterialCompileFailed> ev = std::dynamic_pointer_cast<EffectMaterialCompileFailed, IEvent>(e);
    if (!ev) return;
    Enigma::Platforms::Debug::ErrorPrintf("effect material %s compile failed : %s\n", ev->GetFilename().c_str(), ev->GetErrorCode().message().c_str());
}

void EffectMaterialManagerTest::OnVertexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<VertexBufferBuilt> ev = std::dynamic_pointer_cast<VertexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != VertexBufferName) return;
    m_vtxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IVertexBufferPtr>(ev->GetBufferName());
}

void EffectMaterialManagerTest::OnIndexBufferBuilt(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<IndexBufferBuilt> ev = std::dynamic_pointer_cast<IndexBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetBufferName() != IndexBufferName) return;
    m_idxBuffer = IGraphicAPI::Instance()->GetGraphicAsset<IIndexBufferPtr>(ev->GetBufferName());
}

void EffectMaterialManagerTest::OnTextureLoaded(const IEventPtr& e)
{
    if (!e) return;
    std::shared_ptr<TextureLoaded> ev = std::dynamic_pointer_cast<TextureLoaded, IEvent>(e);
    if (!ev) return;
    if (ev->GetTextureName() != TextureName) return;
    m_texture = IGraphicAPI::Instance()->GetGraphicAsset<ITexturePtr>(ev->GetTextureName());
    BuildVariables();
}

void EffectMaterialManagerTest::BuildVariables()
{
    if ((!m_material) || (!m_texture)) return;

    auto vars = m_material->GetEffectVariablesByName("DiffuseTexture");
    if (!vars.empty())
    {
        for (auto& var : vars)
        {
            var.get().AssignValue(IShaderVariable::TextureVarTuple{ m_texture, std::nullopt });
        }
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopLeftName, "offset_x"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopLeftName, "offset_y"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomLeftName, "offset_x"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomLeftName, "offset_y"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopRightName, "offset_x"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassTopRightName, "offset_y"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomRightName, "offset_x"))
    {
        offset_var.value().get().AssignValue(0.3f);
    }
    if (auto offset_var = m_material->GetEffectVariableInPassByName(PassBottomRightName, "offset_y"))
    {
        offset_var.value().get().AssignValue(-0.3f);
    }
}
