#include "GeometryDataGatewayTest.h"
#include "Controllers/InstallingPolicies.h"
#include "MathLib/MathRandom.h"
#include "GameEngine/GeometryRepository.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/TriangleList.h"
#include "GameEngine/GeometryDataContract.h"
#include "GameEngine/ContractCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ColorRGBA.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "Gateways/ContractJsonGateway.h"
#include "FileSystem/Filename.h"
#include "FileSystem/IFile.h"
#include "FileSystem/FileSystem.h"
#include <cassert>
#include <FileSystem/IFile.h>

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

}

void GeometryDataGatewayTest::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));

    m_onContractedGeometryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnContractedGeometryCreated(e); });
    EventPublisher::Subscribe(typeid(ContractedGeometryCreated), m_onContractedGeometryCreated);

    MathRandom::RandomSeed();

    MakeGeometrySaved();
    LoadGeometry();
}

void GeometryDataGatewayTest::ShutdownEngine()
{
    EventPublisher::Unsubscribe(typeid(ContractedGeometryCreated), m_onContractedGeometryCreated);
    m_onContractedGeometryCreated = nullptr;

    m_graphicMain->ShutdownRenderEngine();
}

void GeometryDataGatewayTest::OnGeometryDataBuilt(const Enigma::Frameworks::IEventPtr& e)
{

}

void GeometryDataGatewayTest::OnContractedGeometryCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ContractedGeometryCreated, IEvent>(e);
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
    TriangleListContract contract;
    contract.Name() = GeometryName;
    contract.VertexFormat() = "xyz_nor_tex1(2)_fdiffuse";
    contract.Segments() = { 0, 0, 4, 4 };
    for (int i = 0; i < 4; i++)
    {
        m_uvs.emplace_back(one_rand(), one_rand());
        m_positions.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_normals.emplace_back(ten_rand(), ten_rand(), ten_rand());
        m_diffuses.emplace_back(one_rand(), one_rand(), one_rand(), one_rand());
    }
    TextureCoordContract texture_coord_contract;
    texture_coord_contract.Texture2DCoords() = m_uvs;
    contract.TextureCoords().emplace_back(texture_coord_contract.ToContract());
    contract.Position3s() = m_positions;
    contract.Normals() = m_normals;
    contract.DiffuseColors() = m_diffuses;
    contract.Indices() = m_indices;
    contract.VertexCapacity() = 4;
    contract.VertexUsedCount() = 4;
    contract.IndexCapacity() = 4;
    contract.IndexUsedCount() = 4;
    contract.Topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    BoundingVolume bound{ Box3::UNIT_BOX };
    contract.GeometryBound() = bound.SerializeContract().ToContract();

    Contract gen_contract = contract.ToContract();
    FactoryDesc desc(TriangleList::TYPE_RTTI.GetName());
    desc.ClaimAsResourceAsset(GeometryName, "test_geometry.geo");
    gen_contract.AddRtti(desc);
    std::string json = ContractJsonGateway::Serialize(std::vector<Contract>{gen_contract});

    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("test_geometry.geo"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    //TriangleListPtr geometry = repository->Create();
}

void GeometryDataGatewayTest::LoadGeometry()
{
    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename("test_geometry.geo"), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    std::vector<Contract> read_contracts = ContractJsonGateway::Deserialize(read_json);
    assert(read_contracts.size() == 1);
    CommandBus::Post(std::make_shared<InvokeContractFactory>(read_contracts[0]));
}
