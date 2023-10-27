#include "SceneGraphPanel.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"
#include "LevelEditorCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "LevelEditorEvents.h"

using namespace LevelEditor;

bool clear_item_value(nana::treebox::item_proxy i)
{
    i.value(nullptr);
    return true;
}

SceneGraphPanel::SceneGraphPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_sceneGraphTree = nullptr;
}

SceneGraphPanel::~SceneGraphPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_sceneGraphTree);
}

void SceneGraphPanel::Initialize(MainForm* main_form)
{
    m_main = main_form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<scene_graph_tree>");
    m_sceneGraphTree = menew nana::treebox{ *this };
    UISchemeColors::ApplySchemaColors(m_sceneGraphTree->scheme());
    m_sceneGraphTree->scheme().item_bg_selected = UISchemeColors::BACKGROUND;
    m_sceneGraphTree->scheme().item_fg_selected = UISchemeColors::FOREGROUND;
    m_sceneGraphTree->scheme().item_bg_highlighted = UISchemeColors::BACKGROUND;
    m_sceneGraphTree->scheme().item_fg_highlighted = UISchemeColors::FOREGROUND;
    m_sceneGraphTree->scheme().item_bg_selected_and_highlighted = UISchemeColors::BACKGROUND;
    m_sceneGraphTree->scheme().item_fg_selected_and_highlighted = UISchemeColors::FOREGROUND;
    //m_sceneGraphTree->scheme().background = UISchemeColors::FOREGROUND;
    m_sceneGraphTree->events().selected([this](const nana::arg_treebox& arg) { this->OnSceneGraphTreeSelected(arg); });
    (*m_place)["scene_graph_tree"] << *m_sceneGraphTree;

    m_place->collocate();
}

void SceneGraphPanel::Finalize()
{
    // treebox clear 並不會清掉 item 裡掛入的物件
    auto item = m_sceneGraphTree->first();
    if (!item.empty()) item.visit_recursively(clear_item_value);
    while (!item.empty())
    {
        item = m_sceneGraphTree->erase(item);
    }
}

void SceneGraphPanel::SubscribeHandlers()
{
    m_doRefreshingSceneGraph = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](auto c) { DoRefreshingSceneGraphTree(c); });
    Enigma::Frameworks::CommandBus::subscribe(typeid(RefreshSceneGraph), m_doRefreshingSceneGraph);
}

void SceneGraphPanel::UnsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::unsubscribe(typeid(RefreshSceneGraph), m_doRefreshingSceneGraph);
    m_doRefreshingSceneGraph = nullptr;
}

void SceneGraphPanel::RefreshSceneGraphTree(const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& flattened_scene)
{
    auto item = m_sceneGraphTree->first();
    if (!item.empty()) item.visit_recursively(clear_item_value);
    while (!item.empty())
    {
        item = m_sceneGraphTree->erase(item);
    }
    if (flattened_scene.empty()) return;

    //m_sceneGraphTree->clear();
    bool is_empty = m_sceneGraphTree->empty();

    std::unordered_map<std::string, nana::treebox::item_proxy> spatial_pos_map;

    unsigned int spatial_count = 0;
    std::shared_ptr scene_root = flattened_scene[0];
    if (!scene_root) return;
    std::string graph_node_name;
    if (!scene_root->GetSpatialName().empty())
    {
        graph_node_name = scene_root->GetSpatialName() + "<" + scene_root->typeInfo().getName() + ">";
    }
    else
    {
        graph_node_name = std::string("untitled") + "<" + scene_root->typeInfo().getName() + ">";
    }
    nana::treebox::item_proxy root_pos = m_sceneGraphTree->insert(scene_root->GetSpatialName(), graph_node_name);
    // 掛入 value object, 統一使用基底 spatial 類別, 但不能用 weak_ptr
    root_pos.value(scene_root);
    spatial_pos_map.emplace(scene_root->GetSpatialName(), root_pos);

    auto iter = flattened_scene.begin();
    ++iter;  // skip scene root
    while (iter != flattened_scene.end())
    {
        if (*iter)
        {
            std::shared_ptr spatial = *iter;
            if (!spatial)
            {
                ++iter;
                continue;
            }
            nana::treebox::item_proxy parent_pos;
            if (spatial->GetParent() == nullptr)
            {
                parent_pos = spatial_pos_map.find(scene_root->GetSpatialName())->second;
            }
            else
            {
                auto parent = spatial->GetParent();
                if (parent)
                {
                    parent_pos = spatial_pos_map.find(parent->GetSpatialName())->second;
                }
                else
                {
                    parent_pos = spatial_pos_map.find(scene_root->GetSpatialName())->second;
                }
            }
            //auto root = parent_pos.value<SpatialPtr>();
            std::string graph_node_name;
            if (!spatial->GetSpatialName().empty())
            {
                graph_node_name = spatial->GetSpatialName() + "<" + spatial->typeInfo().getName() + ">";
            }
            else
            {
                graph_node_name = std::string("untitled") + "<" + spatial->typeInfo().getName() + ">";
            }
            nana::treebox::item_proxy node_pos = m_sceneGraphTree->insert(
                parent_pos, spatial->GetSpatialName(), graph_node_name);
            node_pos.value(spatial);
            spatial_pos_map.emplace(spatial->GetSpatialName(), node_pos);
        }
        ++iter;
    }
}

void SceneGraphPanel::OnSceneGraphTreeSelected(const nana::arg_treebox& arg)
{
    if (!arg.operated) return; // unselected item
    if (arg.item.empty()) return;
    const auto& spatial = arg.item.value<std::shared_ptr<Enigma::SceneGraph::Spatial>>();
    if (!spatial) return;
    Enigma::Frameworks::EventPublisher::post(std::make_shared<PickedSpatialChanged>(spatial, PickedSpatialChanged::PickedFrom::FromSceneGraph));
}

void SceneGraphPanel::DoRefreshingSceneGraphTree(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RefreshSceneGraph, Enigma::Frameworks::ICommand>(c);
    if (!cmd) return;
    RefreshSceneGraphTree(cmd->getFlattenedSpatial());
}
