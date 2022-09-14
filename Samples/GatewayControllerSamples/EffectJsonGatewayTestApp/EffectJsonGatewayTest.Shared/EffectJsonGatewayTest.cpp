#include "EffectJsonGatewayTest.h"
#include <Platforms/MemoryAllocMacro.h>
#include "Controllers/InstallingPolicies.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"

using namespace Enigma::Application;
using namespace Enigma::Controllers;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

std::string PrimaryTargetName = "primary_target";
std::string DefaultRendererName = "default_renderer";

EffectJsonGatewayTest::EffectJsonGatewayTest(const std::string app_name) : AppDelegate(app_name)
{
	
}

EffectJsonGatewayTest::~EffectJsonGatewayTest()
{
	
}

void EffectJsonGatewayTest::InitializeMountPaths()
{
	
}

void EffectJsonGatewayTest::InstallEngine()
{
	assert(m_graphicMain);

	auto creating_policy = std::make_unique<DeviceCreatingPolicy>(Enigma::Graphics::DeviceRequiredBits(), m_hwnd);
	auto policy = std::make_unique<InstallingDefaultRendererPolicy>(std::move(creating_policy), DefaultRendererName, PrimaryTargetName);
	m_graphicMain->InstallRenderEngine(std::move(policy));

	m_gateway = menew EffectPolicyJsonGateway();

    IFilePtr iFile = FileSystem::Instance()->OpenFile("TestEffect.efx", "rb", "");
    if (FATAL_LOG_EXPR(!iFile)) return;
    size_t file_size = iFile->Size();
    if (FATAL_LOG_EXPR(file_size <= 0))
    {
        FileSystem::Instance()->CloseFile(iFile);
        return;
    }

    auto code_buff = iFile->Read(0, file_size);
	if (!code_buff) return;
    FileSystem::Instance()->CloseFile(iFile);
	m_gateway->Deserialize(convert_to_string(code_buff.value(), file_size));
}

void EffectJsonGatewayTest::ShutdownEngine()
{
	delete m_gateway;
	m_gateway = nullptr;
	m_graphicMain->ShutdownRenderEngine();
}

void EffectJsonGatewayTest::FrameUpdate()
{
	
}

void EffectJsonGatewayTest::RenderFrame()
{
	
}
