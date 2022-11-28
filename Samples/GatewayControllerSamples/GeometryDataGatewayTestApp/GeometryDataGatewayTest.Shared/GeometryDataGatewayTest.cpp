#include "GeometryDataGatewayTest.h"
#include "Controllers/InstallingPolicies.h"
#include "MathLib/MathRandom.h"
#include "GameEngine/GeometryRepository.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/TriangleList.h"
#include "GameEngine/GeometryDataDto.h"
#include "GameEngine/FactoryCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "GameEngine/GeometryDataPolicy.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/AsyncJsonFileDtoDeserializer.h"
#include "GameEngine/GeometryCommands.h"
#include <cassert>

using namespace Enigma::Application;
using namespace Enigma::Controllers;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";
std::string GeometryName = "test_geometry";

GeometryDataGatewayTest::GeometryDataGatewayTest(const std::string app_name) : AppDelegate(app_name)
{

}

GeometryDataGatewayTest::~GeometryDataGatewayTest()
{

}

void GeometryDataGatewayTest::InitializeMountPaths()
{
#if TARGET_PLATFORM == PLATFORM_ANDROID
    std::string data_path = Enigma::Platforms::AndroidBridge::GetDataFilePath();
    char s[2048];
    memset(s, 0, 2048);
    memcpy(s, data_path.c_str(), data_path.size());
#else
    std::string data_path = std::filesystem::current_path().string();
#endif
    FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(data_path, "DataPath"));
}

void GeometryDataGatewayTest::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));

    m_onFactoryGeometryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryGeometryCreated(e); });
    EventPublisher::Subscribe(typeid(FactoryGeometryCreated), m_onFactoryGeometryCreated);

    MathRandom::RandomSeed();

    MakeGeometrySaved();
    //LoadGeometry();
    LoadGeometryByPolicy();
}

void GeometryDataGatewayTest::ShutdownEngine()
{
    EventPublisher::Unsubscribe(typeid(FactoryGeometryCreated), m_onFactoryGeometryCreated);
    m_onFactoryGeometryCreated = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void GeometryDataGatewayTest::OnGeometryDataBuilt(const Enigma::Frameworks::IEventPtr& e)
{

}

void GeometryDataGatewayTest::OnFactoryGeometryCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryGeometryCreated, IEvent>(e);
    assert(ev);
    std::shared_ptr<GeometryData> geometry = ev->GetGeometryData();
    for (int i = 0; i < 4; i++)
    {
        assert(m_positions[i] == geometry->GetPosition3(i));
        assert(m_normals[i] == geometry->GetVertexNormal(i));
        assert(m_diffuses[i] == geometry->GetDiffuseColorArray(VertexDescription::ColorNumeric::Float, 4)[i]);
        assert(m_uvs[i] == geometry->GetTexture2DCoordArray(0, 4)[i]);
    }
}

void GeometryDataGatewayTest::MakeGeometrySaved()
{
    UniformFloatDistribution ten_rand = MathRandom::IntervalDistribution(-10.0f, std::nextafter(10.0f, 10.1f));
    UniformFloatDistribution one_rand = MathRandom::IntervalDistribution(0.0f, std::nextafter(1.0f, 1.01f));
    GeometryRepository* repository = ServiceManager::GetSystemServiceAs<GeometryRepository>();
    assert(repository);
    TriangleListDto dto;
    dto.Name() = GeometryName;
    dto.VertexFormat() = "xyz_nor_tex1(2)_fdiffuse";
    dto.Segments() = { 0, 0, 4, 4 };
    for (int i = 0; i < 4; i++)
    {
        m_uvs.emplace_back(one_rand(), one_rand());
        m_positions.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_normals.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_diffuses.emplace_back(one_rand(), one_rand(), one_rand(), one_rand());
    }
    TextureCoordDto texture_coord_dto;
    texture_coord_dto.Texture2DCoords() = m_uvs;
    dto.TextureCoords().emplace_back(texture_coord_dto.ToGenericDto());
    dto.Position3s() = m_positions;
    dto.Normals() = m_normals;
    dto.DiffuseColors() = m_diffuses;
    dto.Indices() = m_indices;
    dto.VertexCapacity() = 4;
    dto.VertexUsedCount() = 4;
    dto.IndexCapacity() = 4;
    dto.IndexUsedCount() = 4;
    dto.Topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    BoundingVolume bound{ Box3::UNIT_BOX };
    dto.GeometryBound() = bound.SerializeDto().ToGenericDto();

    GenericDto gen_dto = dto.ToGenericDto();
    FactoryDesc desc(TriangleList::TYPE_RTTI.GetName());
    desc.ClaimAsResourceAsset(GeometryName, "test_geometry.geo", "DataPath");
    gen_dto.AddRtti(desc);
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto>{gen_dto});

    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("test_geometry.geo@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    //TriangleListPtr geometry = repository->Create();
}

void GeometryDataGatewayTest::LoadGeometry()
{
    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename("test_geometry.geo@DataPath"), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    std::vector<GenericDto> read_dtos = DtoJsonGateway::Deserialize(read_json);
    assert(read_dtos.size() == 1);
    CommandBus::Post(std::make_shared<InvokeDtoFactory>(read_dtos[0]));
}

void GeometryDataGatewayTest::LoadGeometryByPolicy()
{
    //GeometryDataPolicy policy(GeometryName, "test_geometry.geo@DataPath", std::make_shared<JsonFileDtoDeserializer>());
    GeometryDataPolicy policy(GeometryName, "test_geometry.geo@DataPath", std::make_shared<AsyncJsonFileDtoDeserializer>());
    CommandBus::Post(std::make_shared<BuildGeometryData>(policy));
}
