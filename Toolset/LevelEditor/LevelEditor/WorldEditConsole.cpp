#include "WorldEditConsole.h"
#include "Frameworks/CommandBus.h"
#include "LevelEditorCommands.h"
#include "Platforms/PlatformLayer.h"
#include "WorldMap/WorldMapEvents.h"
#include "EditorUtilities.h"
#include "Gateways/DtoJsonGateway.h"
#include "WorldEditService.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Frameworks/EventPublisher.h"
#include <filesystem>

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::FileSystem;
using namespace Enigma::WorldMap;

Rtti WorldEditConsole::TYPE_RTTI{ "LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI };

WorldEditConsole::WorldEditConsole(ServiceManager* srv_mngr, const std::shared_ptr<WorldEditService>& world_service) : ISystemService(srv_mngr)
{
    m_worldEditService = world_service;
    m_needTick = false;
}

WorldEditConsole::~WorldEditConsole()
{
}

ServiceResult WorldEditConsole::onInit()
{
    m_onWorldMapCreated = std::make_shared<EventSubscriber>([=](auto e) { onWorldMapCreated(e); });
    EventPublisher::subscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    return ServiceResult::Complete;
}

ServiceResult WorldEditConsole::onTerm()
{
    EventPublisher::unsubscribe(typeid(WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapCreated = nullptr;
    return ServiceResult::Complete;
}

void WorldEditConsole::setWorldMapRootFolder(const std::filesystem::path& folder, const std::string& world_map_path_id)
{
    auto path = std::filesystem::current_path();
    auto mediaPath = path / "../../../Media/";
    m_mapFileRootPath = mediaPath / folder;
    m_worldMapPathId = world_map_path_id;
    FileSystem::Instance()->AddMountPath(std::make_shared<StdMountPath>(m_mapFileRootPath.string(), world_map_path_id));
}

bool WorldEditConsole::checkWorldMapFolder(const std::string& world_folder)
{
    if (world_folder.empty()) return false;
    const auto world_path = m_mapFileRootPath / world_folder;
    return is_directory(world_path);
}

void WorldEditConsole::deleteWorldMapFolder(const std::string& world_folder)
{
    if (world_folder.empty()) return;
    const auto world_path = m_mapFileRootPath / world_folder;
    std::error_code ec;
    remove_all(world_path, ec);
    if (ec)
    {
        Enigma::Platforms::Debug::ErrorPrintf("remove folder all error : %s", ec.message().c_str());
    }
}

void WorldEditConsole::createWorldMapFolder(const std::string& folder_name)
{
    if (folder_name.empty()) return;
    m_currentWorldFolder = folder_name;
    const auto world_path = m_mapFileRootPath / folder_name;
    if (const bool is_created = create_directory(world_path); is_created)
    {
        CommandBus::post(std::make_shared<OutputMessage>("[WorldEditorService] World map folder " + folder_name + " is created!"));
    }
}

void WorldEditConsole::saveWorldMap()
{
    assert(!m_worldEditService.expired());
    CommandBus::post(std::make_shared<SaveTerrainSplatTexture>(m_worldMapPathId));
    auto [map_graph, node_graphs] = m_worldEditService.lock()->serializeWorldMapAndNodeGraphs(m_worldMapPathId);
    if (!map_graph.empty())
    {
        std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(map_graph);
        IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(map_graph[0].GetRtti().GetPrefab()), Write | OpenAlways | Binary);
        if (FATAL_LOG_EXPR(!iFile)) return;
        iFile->Write(0, convert_to_buffer(json));
        FileSystem::Instance()->CloseFile(iFile);
    }
    for (auto& dtos : node_graphs)
    {
        std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(dtos);
        IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(dtos[0].GetRtti().GetPrefab()), Write | OpenAlways | Binary);
        if (FATAL_LOG_EXPR(!iFile)) return;
        iFile->Write(0, convert_to_buffer(json));
        FileSystem::Instance()->CloseFile(iFile);
    }
}

void WorldEditConsole::loadWorldMap(const std::filesystem::path& map_filepath)
{
    std::string path_string = filePathCombinePathID(map_filepath, m_worldMapPathId) + "@" + m_worldMapPathId;

    Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Load World File " + path_string + "..."));
    IFilePtr iFile = FileSystem::Instance()->OpenFile(path_string, Read | Binary);
    size_t file_size = iFile->Size();

    auto read_buf = iFile->Read(0, file_size);
    FileSystem::Instance()->CloseFile(iFile);
    auto dtos = Enigma::Gateways::DtoJsonGateway::Deserialize(convert_to_string(read_buf.value(), file_size));
    if (!m_worldEditService.expired())
    {
        m_worldEditService.lock()->deserializeWorldMap(dtos);
    }
}

void WorldEditConsole::onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Enigma::WorldMap::WorldMapCreated, IEvent>(e);
    if (!ev) return;
    m_currentWorldName = ev->getName();
}
