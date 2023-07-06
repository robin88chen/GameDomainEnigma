#include "WorldMap.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "WorldMapDto.h"
#include <cassert>

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(WorldMap, WorldMap, PortalZoneNode);

WorldMap::WorldMap(const std::string& name) : PortalZoneNode(name)
{
}

WorldMap::WorldMap(const Engine::GenericDto& o) : PortalZoneNode(o)
{
    WorldMapDto world = WorldMapDto::FromGenericDto(o);
}

WorldMap::~WorldMap()
{
}

void WorldMap::SetPortalRootNode(const std::shared_ptr<PortalManagementNode>& root)
{
    assert(root);
    m_portalRootNode = root;
}
