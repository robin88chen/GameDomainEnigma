#include "AssetsBrowsePanel.h"
#include "EditorUtilities.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "LevelEditorQueries.h"
#include "WorldMap/WorldMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Terrain/TerrainPawn.h"
#include "WorldMap/WorldMapCommands.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/Light.h"

const std::string WORLD_ASSETS_KEY("World_Assets");
const std::string WORLD_ASSETS_NAME("Worlds :");
const std::string PAWN_ASSETS_KEY("Pawn_Assets");
const std::string PAWN_ASSETS_NAME("Pawns :");
const std::string NODE_ASSETS_KEY("Node_Assets");
const std::string NODE_ASSETS_NAME("Nodes :");
const std::string LIGHT_ASSETS_KEY("Light_Assets");
const std::string LIGHT_ASSETS_NAME("Lights :");

using namespace LevelEditor;
AssetsBrowsePanel::AssetsBrowsePanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_assetsTree = nullptr;
    m_popupMenu = nullptr;
}

AssetsBrowsePanel::~AssetsBrowsePanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_assetsTree);
    SAFE_DELETE(m_popupMenu);
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
    m_assetsTree->insert(PAWN_ASSETS_KEY, PAWN_ASSETS_NAME);
    m_assetsTree->insert(NODE_ASSETS_KEY, NODE_ASSETS_NAME);
    m_assetsTree->insert(LIGHT_ASSETS_KEY, LIGHT_ASSETS_NAME);
    m_assetsTree->events().mouse_down([this](const nana::arg_mouse& arg) { this->onAssetsTreeMouseDown(arg); });
    (*m_place)["asset_tree"] << *m_assetsTree;

    m_popupMenu = menew nana::menu{};
    m_popupMenu->append("Remove", [this](auto item) { this->onRemoveAsset(item); });

    m_place->collocate();
}

void AssetsBrowsePanel::finalize()
{
    auto item = m_assetsTree->find(WORLD_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();
    item = m_assetsTree->find(PAWN_ASSETS_KEY);
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
    m_onWorldMapRemoved = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](const Enigma::Frameworks::IEventPtr& e) { onWorldMapRemoved(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::WorldMap::WorldMapRemoved), m_onWorldMapRemoved);
    m_onSpatialRemoved = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](const Enigma::Frameworks::IEventPtr& e) { onSpatialRemoved(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(Enigma::SceneGraph::SpatialRemoved), m_onSpatialRemoved);

    refreshWorldMapAssets();
    refreshPawnAssets();
    refreshNodeAssets();
    refreshLightAssets();
}

void AssetsBrowsePanel::unsubscribeHandlers()
{
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapCreated), m_onWorldMapCreated);
    m_onWorldMapCreated = nullptr;
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::SceneGraph::SpatialConstituted), m_onSpatialConstituted);
    m_onSpatialConstituted = nullptr;
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::WorldMap::WorldMapRemoved), m_onWorldMapRemoved);
    m_onWorldMapRemoved = nullptr;
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(Enigma::SceneGraph::SpatialRemoved), m_onSpatialRemoved);
    m_onSpatialRemoved = nullptr;
}

bool AssetsBrowsePanel::isAssetHovered() const
{
    auto item = m_assetsTree->hovered(true);
    if (item.empty()) return false;
    if (isRootItemOfAssets(item)) return false;
    return true;
}

AssetIdCombo AssetsBrowsePanel::getSelectedAssetId() const
{
    auto item = m_assetsTree->selected();
    if (item.empty()) return AssetIdCombo{};
    if (isRootItemOfAssets(item)) return AssetIdCombo{};
    if (item.key().find_first_of(WORLD_ASSETS_KEY) == 0)
    {
        auto world_map_id = item.value<Enigma::WorldMap::WorldMapId>();
        return AssetIdCombo{ world_map_id };
    }
    else if (item.key().find_first_of(PAWN_ASSETS_KEY) == 0)
    {
        auto pawn_id = item.value<Enigma::SceneGraph::SpatialId>();
        return AssetIdCombo{ pawn_id };
    }
    else if (item.key().find_first_of(NODE_ASSETS_KEY) == 0)
    {
        auto node_id = item.value<Enigma::SceneGraph::SpatialId>();
        return AssetIdCombo{ node_id };
    }
    else if (item.key().find_first_of(LIGHT_ASSETS_KEY) == 0)
    {
        auto light_id = item.value<Enigma::SceneGraph::SpatialId>();
        return AssetIdCombo{ light_id };
    }
    return AssetIdCombo{};
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
        auto item = m_assetsTree->insert(makeWorldMapAssetKey(asset), asset.name());
        item->value(asset);
    }
}

void AssetsBrowsePanel::refreshPawnAssets()
{
    auto item = m_assetsTree->find(PAWN_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();

    auto pawn_assets = std::make_shared<QueryPawnIds>()->dispatch();
    if (pawn_assets.empty()) return;
    for (const auto& asset : pawn_assets)
    {
        auto item = m_assetsTree->insert(makePawnAssetKey(asset), asset.name());
        item->value(asset);
    }
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
        auto item = m_assetsTree->insert(makeNodeAssetKey(asset), asset.name());
        item->value(asset);
    }
}

void AssetsBrowsePanel::refreshLightAssets()
{
    auto item = m_assetsTree->find(LIGHT_ASSETS_KEY);
    if (!item.empty()) item.visit_recursively(clearTreeItemValue);
    item.clear();

    auto light_assets = std::make_shared<QueryLightIds>()->dispatch();
    if (light_assets.empty()) return;
    for (const auto& asset : light_assets)
    {
        auto item = m_assetsTree->insert(makeLightAssetKey(asset), asset.name());
        item->value(asset);
    }
}

void AssetsBrowsePanel::onAssetsTreeMouseDown(const nana::arg_mouse& arg)
{
    if (!arg.right_button) return;
    auto item = m_assetsTree->selected();
    if (item.empty()) return;
    if (isRootItemOfAssets(item)) return;
    auto popuper = nana::menu_popuper(*m_popupMenu);
    popuper(arg);
}

void AssetsBrowsePanel::onRemoveAsset(nana::menu::item_proxy& item)
{
    auto selected = m_assetsTree->selected();
    if (selected.empty()) return;
    if (isRootItemOfAssets(selected)) return;
    if (selected.key().find_first_of(WORLD_ASSETS_KEY) == 0)
    {
        auto world_map_id = selected.value<Enigma::WorldMap::WorldMapId>();
        std::make_shared<Enigma::WorldMap::RemoveWorldMap>(world_map_id)->enqueue();
    }
    else if (selected.key().find_first_of(PAWN_ASSETS_KEY) == 0)
    {
        auto pawn_id = selected.value<Enigma::SceneGraph::SpatialId>();
        std::make_shared<Enigma::SceneGraph::RemoveSpatial>(pawn_id)->enqueue();
    }
    else if (selected.key().find_first_of(NODE_ASSETS_KEY) == 0)
    {
        auto node_id = selected.value<Enigma::SceneGraph::SpatialId>();
        std::make_shared<Enigma::SceneGraph::RemoveSpatial>(node_id)->enqueue();
        if (node_id.rtti().isDerived(Enigma::SceneGraph::LazyNode::TYPE_RTTI))
        {
            std::make_shared<Enigma::SceneGraph::RemoveLaziedContent>(node_id)->enqueue();
        }
    }
    else if (selected.key().find_first_of(LIGHT_ASSETS_KEY) == 0)
    {
        auto light_id = selected.value<Enigma::SceneGraph::SpatialId>();
        std::make_shared<Enigma::SceneGraph::RemoveSpatial>(light_id)->enqueue();
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
    else if (ev->spatial()->typeInfo().isDerived(Enigma::SceneGraph::Pawn::TYPE_RTTI))
    {
        refreshPawnAssets();
    }
    else if (ev->spatial()->typeInfo().isDerived(Enigma::SceneGraph::Light::TYPE_RTTI))
    {
        refreshLightAssets();
    }
}

void AssetsBrowsePanel::onWorldMapRemoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::WorldMap::WorldMapRemoved>(e);
    if (!ev) return;
    refreshWorldMapAssets();
}

void AssetsBrowsePanel::onSpatialRemoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::SceneGraph::SpatialRemoved>(e);
    if (!ev) return;
    if (ev->id().rtti().isDerived(Enigma::SceneGraph::Node::TYPE_RTTI))
    {
        refreshNodeAssets();
    }
    else if (ev->id().rtti().isDerived(Enigma::SceneGraph::Pawn::TYPE_RTTI))
    {
        refreshPawnAssets();
    }
    else if (ev->id().rtti().isDerived(Enigma::SceneGraph::Light::TYPE_RTTI))
    {
        refreshLightAssets();
    }
}

std::string AssetsBrowsePanel::makeWorldMapAssetKey(const Enigma::WorldMap::WorldMapId& world_map_id) const
{
    return WORLD_ASSETS_KEY + "/" + WORLD_ASSETS_KEY + "_" + idToTreeViewKey(world_map_id);
}

std::string AssetsBrowsePanel::makePawnAssetKey(const Enigma::SceneGraph::SpatialId& spatial_id) const
{
    return PAWN_ASSETS_KEY + "/" + PAWN_ASSETS_KEY + "_" + idToTreeViewKey(spatial_id);
}

std::string AssetsBrowsePanel::makeNodeAssetKey(const Enigma::SceneGraph::SpatialId& spatial_id) const
{
    return NODE_ASSETS_KEY + "/" + NODE_ASSETS_KEY + "_" + idToTreeViewKey(spatial_id);
}

std::string AssetsBrowsePanel::makeLightAssetKey(const Enigma::SceneGraph::SpatialId& spatial_id) const
{
    return LIGHT_ASSETS_KEY + "/" + LIGHT_ASSETS_KEY + "_" + idToTreeViewKey(spatial_id);
}

bool AssetsBrowsePanel::isRootItemOfAssets(const nana::treebox::item_proxy& item) const
{
    return item.key() == WORLD_ASSETS_KEY || item.key() == PAWN_ASSETS_KEY || item.key() == NODE_ASSETS_KEY || item.key() == LIGHT_ASSETS_KEY;
}

