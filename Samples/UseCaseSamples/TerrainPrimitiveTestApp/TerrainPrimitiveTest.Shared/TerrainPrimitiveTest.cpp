#include "TerrainPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "FileSystem/AndroidMountPath.h"
#include "Platforms/AndroidBridge.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Frameworks/EventPublisher.h"
#include "Renderer/RendererEvents.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "Terrain/TerrainPrimitiveDto.h"
#include "Terrain/TerrainGeometryDto.h"
#include "Frameworks/CommandBus.h"
#include "Terrain/TerrainInstallingPolicy.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Terrain/TerrainPawn.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/ApplicationBridge.h"
#endif
#include "Gateways/DtoJsonGateway.h"
#include "Controllers/GraphicMain.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "Renderables/RenderablesInstallingPolicy.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/EffectMaterialSourceFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"
#include "Terrain/TerrainGeometry.h"
#include "Controllers/ControllerEvents.h"
#include "CameraMaker.h"
#include "TerrainMaker.h"
#include "GameEngine/TextureDto.h"
#include "GameEngine/Texture.h"

using namespace Enigma::FileSystem;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Gateways;
using namespace Enigma::SceneGraph;
using namespace Enigma::Terrain;
using namespace Enigma::MathLib;
using namespace Enigma::Application;
using namespace Enigma::Controllers;
using namespace Enigma::FileStorage;
using namespace Enigma::Geometries;
using namespace Enigma::Animators;
using namespace Enigma::Primitives;
using namespace Enigma::Renderables;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

TerrainPrimitiveTest::TerrainPrimitiveTest(const std::string& app_name) : AppDelegate(app_name)
{
}

TerrainPrimitiveTest::~TerrainPrimitiveTest()
{
}

void TerrainPrimitiveTest::initializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        auto dataPath = path / "Data";
        auto mediaPath = path / "../../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), MediaPathName));
        if (!std::filesystem::exists(dataPath))
        {
            std::filesystem::create_directory(dataPath);
        }
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(dataPath.string(), "DataPath"));
    }
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    if (FileSystem::instance())
    {
        FileSystem::instance()->addMountPath(std::make_shared<AndroidMountPath>("", MediaPathName));

        std::string data_path = Enigma::Platforms::AndroidBridge::getDataFilePath();
        char s[2048];
        memset(s, 0, 2048);
        memcpy(s, data_path.c_str(), data_path.size());
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(data_path, "DataPath"));
    }
#endif
}

void TerrainPrimitiveTest::installEngine()
{
    m_onRenderEngineInstalled = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderEngineInstalled(e); });
    EventPublisher::subscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRendererCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onRendererCreated(e); });
    EventPublisher::subscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderTargetCreated(e); });
    EventPublisher::subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);

    assert(m_graphicMain);

    auto creating_policy = std::make_shared<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto engine_policy = std::make_shared<EngineInstallingPolicy>();
    auto renderer_policy = std::make_shared<DefaultRendererInstallingPolicy>(DefaultRendererName, PrimaryTargetName);
    auto render_sys_policy = std::make_shared<RenderSystemInstallingPolicy>();
    m_geometryDataFileStoreMapper = std::make_shared<GeometryDataFileStoreMapper>("geometries.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto geometry_policy = std::make_shared<GeometryInstallingPolicy>(m_geometryDataFileStoreMapper);
    m_primitiveFileStoreMapper = std::make_shared<PrimitiveFileStoreMapper>("primitives.db.txt@DataPath", std::make_shared<DtoJsonGateway>());
    auto primitive_policy = std::make_shared<PrimitiveRepositoryInstallingPolicy>(m_primitiveFileStoreMapper);
    auto animator_policy = std::make_shared<AnimatorInstallingPolicy>(std::make_shared<AnimatorFileStoreMapper>("animators.db.txt@DataPath", std::make_shared<DtoJsonGateway>()), std::make_shared<AnimationAssetFileStoreMapper>("animation_assets.db.txt@DataPath", std::make_shared<DtoJsonGateway>()));
    m_sceneGraphFileStoreMapper = std::make_shared<SceneGraphFileStoreMapper>("scene_graph.db.txt@DataPath", "lazy_scene.db.txt@DataPath", "lazied_", std::make_shared<DtoJsonGateway>());
    auto scene_graph_policy = std::make_shared<SceneGraphInstallingPolicy>(m_sceneGraphFileStoreMapper);
    auto effect_material_source_policy = std::make_shared<EffectMaterialSourceRepositoryInstallingPolicy>(std::make_shared<EffectMaterialSourceFileStoreMapper>("effect_materials.db.txt@APK_PATH"));
    m_textureFileStoreMapper = std::make_shared<TextureFileStoreMapper>("textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>());
    auto texture_policy = std::make_shared<TextureRepositoryInstallingPolicy>(m_textureFileStoreMapper);
    auto renderables_policy = std::make_shared<RenderablesInstallingPolicy>();
    auto terrain_policy = std::make_shared<TerrainInstallingPolicy>();
    m_graphicMain->installRenderEngine({ creating_policy, engine_policy, renderer_policy, render_sys_policy, geometry_policy, primitive_policy, animator_policy, scene_graph_policy, effect_material_source_policy, texture_policy, renderables_policy, terrain_policy });
}

void TerrainPrimitiveTest::shutdownEngine()
{
    m_textureFileStoreMapper->removeTexture(m_splatTextureId);
    m_camera = nullptr;
    m_renderer = nullptr;
    m_renderTarget = nullptr;
    m_terrain = nullptr;

    EventPublisher::unsubscribe(typeid(RenderEngineInstalled), m_onRenderEngineInstalled);
    m_onRenderEngineInstalled = nullptr;
    EventPublisher::unsubscribe(typeid(RendererCreated), m_onRendererCreated);
    m_onRendererCreated = nullptr;
    EventPublisher::unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;

    m_graphicMain->shutdownRenderEngine();
}

void TerrainPrimitiveTest::frameUpdate()
{
    AppDelegate::frameUpdate();
    if ((m_renderer) && (m_terrain))
    {
        m_terrain->insertToRendererWithTransformUpdating(m_renderer, Matrix4::IDENTITY, RenderLightingState{});
    }
}

void TerrainPrimitiveTest::renderFrame()
{
    if (!m_renderer) return;
    m_renderer->beginScene();
    m_renderer->clearRenderTarget();
    m_renderer->drawScene();
    m_renderer->endScene();
    m_renderer->flip();
}

void TerrainPrimitiveTest::makeCamera()
{
    m_cameraId = SpatialId("camera", Camera::TYPE_RTTI);
    if (const auto camera = Camera::queryCamera(m_cameraId))
    {
        m_camera = camera;
    }
    else
    {
        m_camera = CameraMaker::makeCamera(m_cameraId);
    }
    if ((m_camera) && (m_renderer)) m_renderer->setAssociatedCamera(m_camera);
}

void TerrainPrimitiveTest::makeTerrain()
{
    auto geo_id = GeometryId("terrain_geo");
    if (!m_geometryDataFileStoreMapper->hasGeometry(geo_id))
    {
        auto geo_dto = TerrainMaker::makeTerrainGeometry(geo_id);
        m_geometryDataFileStoreMapper->putGeometry(geo_id, geo_dto);
    }
    auto prim_id = PrimitiveId("terrain", TerrainPrimitive::TYPE_RTTI);
    if (!m_primitiveFileStoreMapper->hasPrimitive(prim_id))
    {
        m_terrain = TerrainMaker::makeTerrainPrimitive(prim_id, geo_id, m_splatTextureId);
    }
    else
    {
        m_terrain = std::dynamic_pointer_cast<TerrainPrimitive>(Primitive::queryPrimitive(prim_id));
    }
}

void TerrainPrimitiveTest::makeSplatTextureDto()
{
    Enigma::Engine::TextureDto dto;
    m_splatTextureId = Enigma::Engine::TextureId("splat");
    auto asset_filename = m_splatTextureId.name() + ".tex@DataPath";
    auto image_filename = "splat.png@DataPath";
    dto.id() = m_splatTextureId;
    dto.factoryDesc() = Enigma::Engine::FactoryDesc(Enigma::Engine::Texture::TYPE_RTTI.getName()).ClaimAsResourceAsset(m_splatTextureId.name(), asset_filename);
    dto.format() = Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8;
    dto.dimension() = Enigma::MathLib::Dimension<unsigned>{ 512, 512 };
    dto.isCubeTexture() = false;
    dto.surfaceCount() = 1;
    dto.filePaths().push_back(image_filename);
    m_textureFileStoreMapper->putTexture(m_splatTextureId, dto.toGenericDto());
}

void TerrainPrimitiveTest::onRenderEngineInstalled(const IEventPtr& e)
{
    makeCamera();
    makeSplatTextureDto();
    makeTerrain();
}

void TerrainPrimitiveTest::onRendererCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RendererCreated, IEvent>(e);
    if (!ev) return;
    m_renderer = std::dynamic_pointer_cast<Renderer, IRenderer>(ev->renderer());
    m_renderer->setAssociatedCamera(m_camera);
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

void TerrainPrimitiveTest::onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    m_renderTarget = ev->renderTarget();
    if ((m_renderer) && (m_renderTarget)) m_renderer->setRenderTarget(m_renderTarget);
}

/*void TerrainPrimitiveTest::RequestBuildTerrainPrimitive()
{
    TerrainGeometryDto terrain_geometry_dto;
    terrain_geometry_dto.Name() = "terrain_geo";
    terrain_geometry_dto.NumRows() = 10;
    terrain_geometry_dto.NumCols() = 10;
    terrain_geometry_dto.MinPosition() = Vector3(-5.0f, 0.0f, -5.0f);
    terrain_geometry_dto.MaxPosition() = Vector3(5.0f, 0.0f, 5.0f);
    terrain_geometry_dto.MinTextureCoordinate() = Vector2(0.0f, 0.0f);
    terrain_geometry_dto.MaxTextureCoordinate() = Vector2(1.0f, 1.0f);

    TerrainPrimitiveDto terrain_dto;
    EffectMaterialDtoHelper mat_dto("TerrainMesh");
    mat_dto.FilenameAtPath("fx/TerrainMesh.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.TextureMapping("image/du011.png", "APK_PATH", "du011", std::nullopt, "TextureLayer0");
    tex_dto.TextureMapping("image/one.png", "APK_PATH", "one", std::nullopt, "TextureLayer1");
    tex_dto.TextureMapping("image/two.png", "APK_PATH", "two", std::nullopt, "TextureLayer2");
    tex_dto.TextureMapping("image/three.png", "APK_PATH", "three", std::nullopt, "TextureLayer3");
    tex_dto.TextureMapping("splat.png", "DataPath", "splat", std::nullopt, "AlphaLayer");
    terrain_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    terrain_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    terrain_dto.GeometryName() = "terrain_geo";
    terrain_dto.TheGeometry() = terrain_geometry_dto.ToGenericDto();
    GenericDto dto = terrain_dto.ToGenericDto();
    //auto request = std::make_shared<Enigma::Renderer::RequestBuildRenderablePrimitive>(std::dynamic_pointer_cast<RenderablePrimitivePolicy, GenericPolicy>(
      //  terrain_dto.ConvertToPolicy(std::make_shared<Enigma::Gateways::JsonFileDtoDeserializer>())));
    auto request = std::make_shared<Enigma::Renderer::RequestBuildRenderablePrimitive>(std::dynamic_pointer_cast<RenderablePrimitivePolicy, GenericPolicy>(
        dto.ConvertToPolicy(std::make_shared<Enigma::Gateways::JsonFileDtoDeserializer>())));
    m_ruidBuildRequester = request->GetRuid();
    RequestBus::Post(request);
}

void TerrainPrimitiveTest::RequestBuildTerrainPawn()
{
    TerrainGeometryDto terrain_geometry_dto;
    terrain_geometry_dto.Name() = "terrain_geo";
    terrain_geometry_dto.NumRows() = 10;
    terrain_geometry_dto.NumCols() = 10;
    terrain_geometry_dto.MinPosition() = Vector3(-5.0f, 0.0f, -5.0f);
    terrain_geometry_dto.MaxPosition() = Vector3(5.0f, 0.0f, 5.0f);
    terrain_geometry_dto.MinTextureCoordinate() = Vector2(0.0f, 0.0f);
    terrain_geometry_dto.MaxTextureCoordinate() = Vector2(1.0f, 1.0f);

    TerrainPrimitiveDto terrain_dto;
    terrain_dto.Name() = "terrain";
    EffectMaterialDtoHelper mat_dto("TerrainMesh");
    mat_dto.FilenameAtPath("fx/TerrainMesh.efx@APK_PATH");
    EffectTextureMapDtoHelper tex_dto;
    tex_dto.TextureMapping("image/du011.png", "APK_PATH", "du011", std::nullopt, "TextureLayer0");
    tex_dto.TextureMapping("image/one.png", "APK_PATH", "one", std::nullopt, "TextureLayer1");
    tex_dto.TextureMapping("image/two.png", "APK_PATH", "two", std::nullopt, "TextureLayer2");
    tex_dto.TextureMapping("image/three.png", "APK_PATH", "three", std::nullopt, "TextureLayer3");
    tex_dto.TextureMapping("splat.png", "DataPath", "splat", std::nullopt, "AlphaLayer");
    terrain_dto.Effects().emplace_back(mat_dto.ToGenericDto());
    terrain_dto.TextureMaps().emplace_back(tex_dto.ToGenericDto());
    terrain_dto.GeometryName() = "terrain_geo";
    terrain_dto.TheGeometry() = terrain_geometry_dto.ToGenericDto();
    TerrainPawnDtoHelper pawn_dto("TerrainName");
    pawn_dto.TopLevel(true).TerrainPrimitive(terrain_dto);

    std::vector<GenericDto> dtos = { pawn_dto.ToGenericDto() };
    CommandBus::Post(std::make_shared<BuildSceneGraph>("test_scene", dtos));
}*/
