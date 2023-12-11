#include "GeometryRepositoryTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "Gateways/DtoJsonGateway.h"
#include "Frameworks/SystemServiceEvents.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/MathRandom.h"
#include "Geometries/GeometryRepository.h"
#include "Geometries/GeometryDataFactory.h"
#include "Geometries/TriangleList.h"
#include "GameEngine/BoundingVolume.h"
#include "Platforms/AndroidBridge.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Geometries;
using namespace Enigma::MathLib;

std::string GeometryName = "test_geometry";

GeometryRepositoryTest::GeometryRepositoryTest(const std::string app_name) : AppDelegate(app_name)
{
}

GeometryRepositoryTest::~GeometryRepositoryTest()
{
}

void GeometryRepositoryTest::InitializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    std::string data_path = Enigma::Platforms::AndroidBridge::GetDataFilePath();
    char s[2048];
    memset(s, 0, 2048);
    memcpy(s, data_path.c_str(), data_path.size());
#else
    std::string data_path = std::filesystem::current_path().string();
#endif
    FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(data_path, "DataPath"));
}

void GeometryRepositoryTest::InstallEngine()
{
    auto engine_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<Enigma::FileStorage::GeometryDataFileStoreMapper>("geometry_test.db.txt@DataPath",
        std::make_shared<Enigma::Gateways::DtoJsonGateway>()));
    m_graphicMain->installRenderEngine({ engine_policy });

    m_onAllServicesInitialized = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](auto e) { onAllServicesInitialized(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::Frameworks::AllServiceInitialized), m_onAllServicesInitialized);

    MathRandom::RandomSeed();
}

void GeometryRepositoryTest::ShutdownEngine()
{
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::Frameworks::AllServiceInitialized), m_onAllServicesInitialized);
    m_onAllServicesInitialized = nullptr;
    m_graphicMain->shutdownRenderEngine();
}

void GeometryRepositoryTest::onAllServicesInitialized(const Enigma::Frameworks::IEventPtr& e)
{
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    if (!repository->hasGeometryData(GeometryName))
    {
        createNewGeometry();
    }
    else
    {
        queryGeometry();
    }
}

void GeometryRepositoryTest::createNewGeometry()
{
    Enigma::Platforms::Debug::Printf("create new geometry");
    UniformFloatDistribution ten_rand = MathRandom::IntervalDistribution(-10.0f, std::nextafter(10.0f, 10.1f));
    UniformFloatDistribution one_rand = MathRandom::IntervalDistribution(0.0f, std::nextafter(1.0f, 1.01f));
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    TriangleListDto dto;
    dto.id() = GeometryName;
    dto.vertexFormat() = "xyz_nor_tex1(2)_fdiffuse";
    dto.segments() = { 0, 0, 4, 4 };
    for (int i = 0; i < 4; i++)
    {
        m_uvs.emplace_back(one_rand(), one_rand());
        m_positions.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_normals.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_diffuses.emplace_back(one_rand(), one_rand(), one_rand(), one_rand());
    }
    TextureCoordDto texture_coord_dto;
    texture_coord_dto.texture2DCoords() = m_uvs;
    dto.textureCoords().emplace_back(texture_coord_dto.toGenericDto());
    dto.position3s() = m_positions;
    dto.normals() = m_normals;
    dto.diffuseColors() = m_diffuses;
    dto.indices() = m_indices;
    dto.vertexCapacity() = 4;
    dto.vertexUsedCount() = 4;
    dto.indexCapacity() = 4;
    dto.indexUsedCount() = 4;
    dto.topology() = static_cast<unsigned>(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    Enigma::Engine::BoundingVolume bound{ Box3::UNIT_BOX };
    dto.geometryBound() = bound.serializeDto().toGenericDto();
    dto.factoryDesc() = Enigma::Engine::FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(GeometryName, GeometryName + ".geo", "DataPath");

    auto geometry = repository->factory()->constitute(dto.id(), dto.toGenericDto());
    repository->putGeometryData(dto.id(), geometry);
}

void GeometryRepositoryTest::queryGeometry()
{
    Enigma::Platforms::Debug::Printf("query geometry");
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    auto geometry = repository->queryGeometryData(GeometryName);
}

