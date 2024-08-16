#include "GeometryAssemblerTest.h"
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
#include "Geometries/TriangleListAssembler.h"
#include "Geometries/GeometryId.h"
#include "Geometries/GeometryDataQueries.h"
#include "Geometries/StandardGeometryAssemblers.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Geometries;
using namespace Enigma::MathLib;

std::string GeometryName = "test_geometry";

GeometryAssemblerTest::GeometryAssemblerTest(const std::string app_name) : AppDelegate(app_name)
{
}

GeometryAssemblerTest::~GeometryAssemblerTest()
{
}

void GeometryAssemblerTest::initializeMountPaths()
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

void GeometryAssemblerTest::installEngine()
{
    auto engine_policy = std::make_shared<GeometryInstallingPolicy>(std::make_shared<Enigma::FileStorage::GeometryDataFileStoreMapper>("geometry_test.db.txt@DataPath",
        std::make_shared<Enigma::Gateways::DtoJsonGateway>()));
    m_graphicMain->installRenderEngine({ engine_policy });

    m_onAllServicesInitialized = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](auto e) { onAllServicesInitialized(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::Frameworks::AllServiceInitialized), m_onAllServicesInitialized);

    MathRandom::RandomSeed();
}

void GeometryAssemblerTest::shutdownEngine()
{
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::Frameworks::AllServiceInitialized), m_onAllServicesInitialized);
    m_onAllServicesInitialized = nullptr;
    m_graphicMain->shutdownRenderEngine();
}

void GeometryAssemblerTest::onAllServicesInitialized(const Enigma::Frameworks::IEventPtr& e)
{
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    if (!repository->hasGeometryData(GeometryName))
    {
        assembleStandardGeometry();
        //createNewGeometry();
    }
    else
    {
        //queryGeometry();
    }
}

void GeometryAssemblerTest::createNewGeometry()
{
    Enigma::Platforms::Debug::Printf("create new geometry");
    UniformFloatDistribution ten_rand = MathRandom::IntervalDistribution(-10.0f, std::nextafter(10.0f, 10.1f));
    UniformFloatDistribution one_rand = MathRandom::IntervalDistribution(0.0f, std::nextafter(1.0f, 1.01f));
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    std::shared_ptr<TriangleListAssembler> assembler = std::make_shared<TriangleListAssembler>(GeometryId(GeometryName));
    assembler->addSegment({ 0, 4, 0, 4 });
    for (int i = 0; i < 4; i++)
    {
        m_uvs.emplace_back(one_rand(), one_rand());
        m_positions.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_normals.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_diffuses.emplace_back(one_rand(), one_rand(), one_rand(), one_rand());
    }
    assembler->addTexture2DCoords(m_uvs);
    assembler->position3s(m_positions);
    assembler->normals(m_normals);
    assembler->diffuseColors(m_diffuses);
    assembler->indices(m_indices);
    assembler->vertexCapacity(4);
    assembler->vertexUsedCount(4);
    assembler->indexCapacity(4);
    assembler->indexUsedCount(4);
    Enigma::Engine::BoundingVolume bound{ Box3::UNIT_BOX };
    assembler->geometryBound(bound);
    assembler->asAsset(GeometryName, GeometryName + ".geo", "DataPath");
    // 先測試 Gateway
    std::shared_ptr<Enigma::Gateways::IDtoGateway> gateway = std::make_shared<Enigma::Gateways::DtoJsonGateway>();
    std::string json = gateway->serialize(std::vector<Enigma::Engine::GenericDto>{assembler->assemble()});
    IFilePtr iFile = FileSystem::instance()->openFile(Filename(GeometryName + ".geo@DataPath"), write | openAlways);
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->write(0, convert_to_buffer(json));
    FileSystem::instance()->closeFile(iFile);

    //std::shared_ptr<GeometryData> geometry = std::make_shared<RequestGeometryConstitution>(GeometryId(GeometryName), assembler->assemble())->dispatch();
    //repository->putGeometryData(geometry->id(), geometry);
}

void GeometryAssemblerTest::queryGeometry()
{
    Enigma::Platforms::Debug::Printf("query geometry");
    auto repository = m_graphicMain->getSystemServiceAs<GeometryRepository>();
    assert(repository);
    auto geometry = repository->queryGeometryData(GeometryName);
}

void GeometryAssemblerTest::assembleStandardGeometry()
{
    CubeAssembler cube_assembler{ GeometryId(GeometryName) };
    cube_assembler.cube(Vector3::ZERO, Vector3(1.0f, 1.0f, 1.0f)).normal().textureCoord(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f)).asAsset(GeometryName, GeometryName + ".geo", "DataPath");
    // 先測試 Gateway
    std::shared_ptr<Enigma::Gateways::IDtoGateway> gateway = std::make_shared<Enigma::Gateways::DtoJsonGateway>();
    std::string json = gateway->serialize(std::vector<Enigma::Engine::GenericDto>{cube_assembler.assemble()});
    IFilePtr iFile = FileSystem::instance()->openFile(Filename(GeometryName + ".geo@DataPath"), write | openAlways);
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->write(0, convert_to_buffer(json));
    FileSystem::instance()->closeFile(iFile);
}

