#include "WorldEditConsole.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "EditorUtilities.h"
#include "WorldMap/WorldMapQueries.h"
#include "WorldEditService.h"
#include "WorldMap/WorldMapCommands.h"
#include "LevelEditorQueries.h"
#include "LevelEditorCommands.h"
#include "SceneGraph/SceneGraphCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti WorldEditConsole::TYPE_RTTI{ "LevelEditor.WorldEditConsole", &ISystemService::TYPE_RTTI };

WorldEditConsole::WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldEditService>& world_edit_service, const std::string& world_relate_path, const std::string& media_path_id) : ISystemService(srv_mngr), m_worldEditService(world_edit_service), m_worldRelativePath(world_relate_path), m_mediaPathId(media_path_id)
{
    m_needTick = false;
}

WorldEditConsole::~WorldEditConsole()
{
}

Enigma::Frameworks::ServiceResult WorldEditConsole::onInit()
{
    return ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult WorldEditConsole::onTerm()
{
    return ServiceResult::Complete;
}

void WorldEditConsole::saveWorldMap()
{
    auto world_edit_service = m_worldEditService.lock();
    if (!world_edit_service) return;

    auto world_map = world_edit_service->getWorldMap();
    if (!world_map) return;
    std::make_shared<Enigma::WorldMap::PutWorldMap>(world_map->id(), world_map)->execute();
}

void WorldEditConsole::loadWorldMap(const std::filesystem::path& filepath, const Enigma::SceneGraph::SpatialId& root_id)
{
    assert(!root_id.empty());
    auto filename = filePathCombinePathID(filepath, m_mediaPathId);
    if (filename.empty()) return;
    filename = filename + "@" + m_mediaPathId;
    auto world_map_id = std::make_shared<ResolveWorldId>(filename)->dispatch();
    if (!world_map_id.has_value())
    {
        std::make_shared<OutputMessage>("can't resolve world map id from filename " + filename)->enqueue();
        return;
    }
    auto world_map = std::make_shared<Enigma::WorldMap::QueryWorldMap>(world_map_id.value())->dispatch();
    if (!world_map)
    {
        std::make_shared<OutputMessage>("can't load world map " + filename)->enqueue();
        return;
    }
    auto out_region = std::make_shared<Enigma::SceneGraph::QuerySpatial>(world_map->outRegionId())->dispatch();
    if (!out_region)
    {
        std::make_shared<OutputMessage>("can't load out region " + world_map->outRegionId().name())->enqueue();
        return;
    }
    std::make_shared<Enigma::SceneGraph::AttachNodeChild>(root_id, out_region, Enigma::MathLib::Matrix4::IDENTITY)->enqueue();
}

