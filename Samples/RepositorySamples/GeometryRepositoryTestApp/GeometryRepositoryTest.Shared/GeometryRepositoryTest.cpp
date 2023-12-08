#include "GeometryRepositoryTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "GameEngine/EngineInstallingPolicy.h"

using namespace Enigma::Application;
using namespace Enigma::FileSystem;
using namespace Enigma::Engine;

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
    auto engine_policy = std::make_shared<EngineInstallingPolicy>(std::make_shared<JsonFileEffectProfileDeserializer>());

}

void GeometryRepositoryTest::ShutdownEngine()
{

}
