#include "SceneGraphJsonGatewayTest.h"
#include "Controllers/InstallingPolicies.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/ServiceManager.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/SceneFlattenTraversal.h"
#include "SceneGraph/SceneGraphContracts.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Matrix4.h"
#include "MathLib/Vector3.h"
#include "MathLib/MathRandom.h"
#include "Platforms/MemoryAllocMacro.h"
#include "GameEngine/Contract.h"
#include "Gateways/ContractJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/ContractCommands.h"
#include <memory>
#include <cassert>
#include <string>
#include <random>
#include <vector>

using namespace Enigma::Application;
using namespace Enigma::Controllers;
using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

SceneGraphJsonGatewayTest::SceneGraphJsonGatewayTest(const std::string app_name) : AppDelegate(app_name)
{

}

SceneGraphJsonGatewayTest::~SceneGraphJsonGatewayTest()
{

}

void SceneGraphJsonGatewayTest::InitializeMountPaths()
{

}

void SceneGraphJsonGatewayTest::InstallEngine()
{
    assert(m_graphicMain);

    auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
    auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
    m_graphicMain->InstallRenderEngine(std::move(policy));

    MathRandom::RandomSeed();
    UniformFloatDistribution unif_rand = MathRandom::IntervalDistribution(-10.0f, std::nextafter(10.0f, 10.1f));
    UniformFloatDistribution half_pi_rand = MathRandom::IntervalDistribution(-Math::HALF_PI, Math::HALF_PI);
    float yaw = half_pi_rand();
    float pitch = half_pi_rand();
    float roll = half_pi_rand();
    Matrix4 mx_rot;
    mx_rot = Matrix4::MakeRotationYawPitchRoll(yaw, pitch, roll);
    Vector3 vpos = Vector3(unif_rand(), unif_rand(), unif_rand());
    Vector3 vscale = Vector3(unif_rand(), unif_rand(), unif_rand());
    Matrix4 mx_pos;
    mx_pos = Matrix4::MakeTranslateTransform(vpos);
    Matrix4 mx_scale;
    mx_scale = Matrix4::MakeScaleTransform(vscale);
    Matrix4 mx_child1 = mx_pos * mx_rot * mx_scale;

    yaw = half_pi_rand();
    pitch = half_pi_rand();
    roll = half_pi_rand();
    mx_rot = Matrix4::MakeRotationYawPitchRoll(yaw, pitch, roll);
    vpos = Vector3(unif_rand(), unif_rand(), unif_rand());
    vscale = Vector3(unif_rand(), unif_rand(), unif_rand());
    mx_pos = Matrix4::MakeTranslateTransform(vpos);
    mx_scale = Matrix4::MakeScaleTransform(vscale);
    Matrix4 mx_child2 = mx_pos * mx_rot * mx_scale;

    SceneGraphRepository* repository = ServiceManager::GetSystemServiceAs<SceneGraphRepository>();
    std::shared_ptr<Node> root_node = repository->CreateNode("scene_root");
    std::shared_ptr<Node> child1 = repository->CreateNode("child1");
    std::shared_ptr<Node> child2 = repository->CreateNode("child2");

    root_node->AttachChild(child1, mx_child1);
    root_node->AttachChild(child2, mx_child2);
    Matrix4 mx_child2_world = child2->GetWorldTransform();
    SceneFlattenTraversal* flatten = menew SceneFlattenTraversal();
    auto travel = root_node->VisitBy(flatten);
    assert(travel != SceneTraveler::TravelResult::InterruptError);
    std::vector<Contract> contracts;
    for (auto& sp : flatten->GetSpatials())
    {
        if (auto sp_node = std::dynamic_pointer_cast<Node, Spatial>(sp))
        {
            contracts.emplace_back(sp_node->SerializeContract().ToContract());
        }
    }
    std::string json = ContractJsonGateway::Serialize(contracts);
    delete flatten;

    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("scene_graph.ctt"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);

    root_node = nullptr;

    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename("scene_graph.ctt"), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    assert(json == read_json);
    std::vector<Contract> read_contracts = ContractJsonGateway::Deserialize(read_json);
    for (auto& contract : read_contracts)
    {
        CommandBus::Post(std::make_shared<InvokeContractFactory>(contract));
    }
}

void SceneGraphJsonGatewayTest::ShutdownEngine()
{
    m_graphicMain->ShutdownRenderEngine();
}
