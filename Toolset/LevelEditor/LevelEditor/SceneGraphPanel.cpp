#include "SceneGraphPanel.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"

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

void SceneGraphPanel::RefreshSceneGraphTree()
{
}

void SceneGraphPanel::OnSceneGraphTreeSelected(const nana::arg_treebox& arg)
{
}
