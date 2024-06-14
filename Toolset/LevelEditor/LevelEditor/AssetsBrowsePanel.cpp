#include "AssetsBrowsePanel.h"
#include "EditorUtilities.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "LevelEditorQueries.h"
#include "WorldMap/WorldMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Terrain/TerrainPawn.h"

const std::string WORLD_ASSETS_KEY("World_Assets");
const std::string WORLD_ASSETS_NAME("Worlds :");
const std::string TERRAIN_ASSETS_KEY("Terrain_Assets");
const std::string TERRAIN_ASSETS_NAME("Terrains :");
const std::string NODE_ASSETS_KEY("Node_Assets");
const std::string NODE_ASSETS_NAME("Nodes :");

using namespace LevelEditor;
AssetsBrowsePanel::AssetsBrowsePanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_assetsTree = nullptr;
}

AssetsBrowsePanel::~AssetsBrowsePanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_assetsTree);
}

void AssetsBrowsePanel::initialize(MainForm* main_form)
{
    m_mainForm = main_form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<asset_tree>");
    m_assetsTree = menew nana::treebox{ *this };
    UISchemeColors::applySchemaColors(m_assetsTree->scheme());
    m_assetsTree->scheme().item_bg_selected = UISchemeColors::BACKGROUND;
    m_assetsTree->scheme().item_fg_selected = UISchemeColors::FOREGROUND;
    m_assetsTree->scheme().item_bg_highlighted = UISchemeColors::BACKGROUND;
    m_assetsTree->scheme().item_fg_highlighted = UISchemeColors::FOREGROUND;
    m_assetsTree->scheme().item_bg_selected_and_highlighted = UISchemeColors::BACKGROUND;
    m_assetsTree->scheme().item_fg_selected_and_highlighted = UISchemeColors::FOREGROUND;
    m_assetsTree->insert(WORLD_ASSETS_KEY, WORLD_ASSETS_NAME);
    m_assetsTree->insert(TERRAIN_ASSETS_KEY, TERRAIN_ASSETS_NAME);
    m_assetsTree->insert(NODE_ASSETS_KEY, NODE_ASSETS_NAME);
    (*m_place)["asset_tree"] << *m_assetsTree;

    m_place->collocate();
}

void AssetsBrowsePanel::finalize()
{
    auto item = m_assetsTree->find(WORLD_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();
    item = m_assetsTree->find(TERRAIN_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();
    item = m_assetsTree->find(NODE_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();
}

void AssetsBrowsePanel::subscribeHandlers()
{
    m_onWorldMapCreated = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](const Enigma::Frameworks::IEventPtr& e) { onWorldMapCreated(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::WorldMap::WorldMapCreated), m_onWorldMapCreated);
    m_onSpatialConstituted = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](const Enigma::Frameworks::IEventPtr& e) { onSpatialConstituted(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::SceneGraph::SpatialConstituted), m_onSpatialConstituted);

    refreshWorldMapAssets();
    refreshTerrainAssets();
    refreshNodeAssets();
}

void AssetsBrowsePanel::unsubscribeHandlers()
{
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapCreated = nullptr;
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::SceneGraph::SpatialConstituted), m_onSpatialConstituted);
    m_onSpatialConstituted = nullptr;
}

void AssetsBrowsePanel::refreshWorldMapAssets()
{
    auto item = m_assetsTree->find(WORLD_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();

    auto world_assets = std::make_shared<QueryWorldMapIds>()->dispatch();
    if (world_assets.empty()) return;
    for (const auto& asset : world_assets)
    {
        auto item = m_assetsTree->insert(WORLD_ASSETS_KEY + "/" + idToTreeViewKey(asset), asset.name());
        item->value(asset);
    }
}

void AssetsBrowsePanel::refreshTerrainAssets()
{
    auto item = m_assetsTree->find(TERRAIN_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();

    /*auto terrain_assets = std::make_shared<QueryTerrainIds>()->dispatch();
    if (terrain_assets.empty()) return;
    for (const auto& asset : terrain_assets)
    {
        auto item = m_assetsTree->insert(TERRAIN_ASSETS_KEY + "/" + idToTreeViewKey(asset), asset.name());
        item->value(asset);
    }*/
}

void AssetsBrowsePanel::refreshNodeAssets()
{
    auto item = m_assetsTree->find(NODE_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();

    auto node_assets = std::make_shared<QueryNodeIds>()->dispatch();
    if (node_assets.empty()) return;
    for (const auto& asset : node_assets)
    {
        auto item = m_assetsTree->insert(NODE_ASSETS_KEY + "/" + idToTreeViewKey(asset), asset.name());
        item->value(asset);
    }
}

void AssetsBrowsePanel::onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::WorldMap::WorldMapCreated>(e);
    if (!ev) return;
    refreshWorldMapAssets();
}

void AssetsBrowsePanel::onSpatialConstituted(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::SpatialConstituted>(e);
    if (!ev) return;
    if (ev->isPersisted()) return; // this is not new construction
    if (ev->spatial()->typeInfo().isDerived(Enigma::SceneGraph::Node::TYPE_RTTI))
    {
        refreshNodeAssets();
    }
    else if (ev->spatial()->typeInfo().isDerived(Enigma::Terrain::TerrainPawn::TYPE_RTTI))
    {
        refreshTerrainAssets();
    }
}

