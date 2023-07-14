#include "WorldEditConsole.h"
#include "Frameworks/CommandBus.h"
#include "LevelEditorCommands.h"
#include "Platforms/PlatformLayer.h"
#include <filesystem>
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;

Rtti WorldEditConsole::TYPE_RTTI{"LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI};

WorldEditConsole::WorldEditConsole(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
}

WorldEditConsole::~WorldEditConsole()
{
}

ServiceResult WorldEditConsole::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult WorldEditConsole::OnTerm()
{
    return ServiceResult::Complete;
}

void WorldEditConsole::SetWorldMapRootFolder(const std::filesystem::path& folder, const std::string& world_map_path_id)
{
    auto path = std::filesystem::current_path();
    auto mediaPath = path / "../../../Media/";
    m_mapFileRootPath = mediaPath / folder;
    FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(m_mapFileRootPath.string(), world_map_path_id));
}

bool WorldEditConsole::CheckWorldMapFiles(const std::string& world_name)
{
    if (world_name.empty()) return false;
    const auto world_path = m_mapFileRootPath / world_name;
    return is_directory(world_path);
}

void WorldEditConsole::DeleteWorldMapFiles(const std::string& world_name)
{
    if (world_name.empty()) return;
    const auto world_path = m_mapFileRootPath / world_name;
    std::error_code ec;
    remove_all(world_path, ec);
    if (ec)
    {
        Enigma::Platforms::Debug::ErrorPrintf("remove folder all error : %s", ec.message().c_str());
    }
}

void WorldEditConsole::CreateWorldMapFiles(const std::string& world_name)
{
    if (world_name.empty()) return;
    m_currentWorldName = world_name;
    const auto world_path = m_mapFileRootPath / world_name;
    if (const bool is_created = create_directory(world_path); is_created)
    {
        CommandBus::Post(std::make_shared<OutputMessage>("[WorldEditorService] World map folder " + world_name + " is created!"));
    }
}
