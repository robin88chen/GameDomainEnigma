#include "ModelInfoPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"

using namespace EnigmaViewer;

ModelInfoPanel::ModelInfoPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_modelNamePrompt = nullptr;
    m_modelNameLabel = nullptr;
    m_meshNodeTree = nullptr;
    m_popupMenu = nullptr;
}

ModelInfoPanel::~ModelInfoPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_modelNamePrompt);
    SAFE_DELETE(m_modelNameLabel);
    SAFE_DELETE(m_meshNodeTree);
    SAFE_DELETE(m_popupMenu);
}

void ModelInfoPanel::Initialize(MainForm* main_form)
{
    m_main = main_form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<modelnameprompt weight=28><modelname weight=28><meshnodetree>");
    m_modelNamePrompt = menew nana::label{ *this, "Model Name" };
    UISchemeColors::ApplySchemaColors(m_modelNamePrompt->scheme());
    m_modelNameLabel = menew nana::label{ *this, "xxx" };
    UISchemeColors::ApplySchemaColors(m_modelNameLabel->scheme());
    m_meshNodeTree = menew nana::treebox{ *this };
    m_meshNodeTree->events().mouse_down([this](const nana::arg_mouse& arg) { this->OnMeshNodeTreeMouseDown(arg); });
    UISchemeColors::ApplySchemaColors(m_meshNodeTree->scheme());
    m_meshNodeTree->scheme().item_bg_highlighted = UISchemeColors::HIGHLIGHT_BG;
    m_meshNodeTree->scheme().item_bg_selected = UISchemeColors::SELECT_BG;
    (*m_place)["modelnameprompt"] << *m_modelNamePrompt;
    (*m_place)["modelname"] << *m_modelNameLabel;
    (*m_place)["meshnodetree"] << *m_meshNodeTree;

    m_place->collocate();

    m_popupMenu = menew nana::menu{};
    m_popupMenu->append("Add Particles");
    m_popupMenu->append_splitter();
    m_popupMenu->append("Avatar Command");
    nana::menu* particleMenu = m_popupMenu->create_sub_menu(0);
    particleMenu->append("Cloud Particle", [this](auto item) { this->OnAddCloudParticle(item); });
    particleMenu->append("Super Spray Particle", [this](auto item) { this->OnAddSuperSprayParticle(item); });
    nana::menu* avatarMenu = m_popupMenu->create_sub_menu(2);
    avatarMenu->append("Change Texture", [this](auto item) { this->OnChangeMeshTexture(item); });
    //m_meshNodeTree->events().mouse_down(nana::menu_popuper(*m_popupParticleMenu));
}

void ModelInfoPanel::SetModelFileName(const std::string& filename)
{
    if (m_modelNameLabel) m_modelNameLabel->caption(filename);
}

/*void ModelInfoPanel::EnumModelMeshNode(const Enigma::ModelPrimitivePtr& model)
{
    m_meshNodeTree->clear();
    if (!model) return;
    if (!model->GetMeshNodeTree()) return;
    if (model->GetMeshNodeTree()->GetMeshNodeCount() == 0) return;

    std::unordered_map<std::string, nana::treebox::item_proxy> mesh_pos_map;

    unsigned int total_node_count = model->GetMeshNodeTree()->GetMeshNodeCount();
    unsigned int total_mesh_count = model->GetMeshPrimitiveCount();
    unsigned int mesh_count = 0;

    nana::treebox::item_proxy root_pos = m_meshNodeTree->insert("root", model->GetModelName());
    mesh_pos_map.emplace("root", root_pos);
    for (unsigned int ni = 0; ni < total_node_count; ni++)
    {
        Enigma::MeshNodePtr mesh_node = model->GetMeshNodeTree()->GetMeshNode(ni);
        if (!mesh_node) continue;
        nana::treebox::item_proxy parent_pos;
        if (mesh_node->GetParentIndexInArray() == Enigma::MeshNode::MeshNode_Invalid_Index)
        {
            parent_pos = mesh_pos_map.find("root")->second;
        }
        else
        {
            unsigned int parent_idx = mesh_node->GetParentIndexInArray();
            Enigma::MeshNodePtr parent_mesh_node = model->GetMeshNodeTree()->GetMeshNode(parent_idx);
            if (parent_mesh_node)
            {
                parent_pos = mesh_pos_map.find(parent_mesh_node->GetName())->second;
            }
            else
            {
                parent_pos = mesh_pos_map.find("root")->second;
            }
        }
        nana::treebox::item_proxy node_pos = m_meshNodeTree->insert(
            parent_pos, mesh_node->GetName(), mesh_node->GetName());
        mesh_pos_map.emplace(mesh_node->GetName(), node_pos);
        if (mesh_node->GetMeshPrimitive())
        {
            m_meshNodeTree->insert(node_pos,
                mesh_node->GetMeshPrimitive()->GetMeshName(), mesh_node->GetMeshPrimitive()->GetMeshName());
        }
    }
}*/

void ModelInfoPanel::OnMeshNodeTreeMouseDown(const nana::arg_mouse& arg)
{
    if (!arg.right_button) return;
    if (m_meshNodeTree->selected().empty()) return;
    auto poper = menu_popuper(*m_popupMenu);
    poper(arg);
}

void ModelInfoPanel::OnAddCloudParticle(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    //m_main->GetAppDelegate()->AddCloudParticleToMeshNode(m_meshNodeTree->selected().key());
}

void ModelInfoPanel::OnAddSuperSprayParticle(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    //m_main->GetAppDelegate()->AddSprayParticleToMeshNode(m_meshNodeTree->selected().key());
}

void ModelInfoPanel::OnChangeMeshTexture(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    //m_main->GetAppDelegate()->ChangeMeshTexture(m_meshNodeTree->selected().key());
}
