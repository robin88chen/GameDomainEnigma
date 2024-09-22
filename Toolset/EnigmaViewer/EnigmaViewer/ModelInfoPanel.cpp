#include "ModelInfoPanel.h"

#include <Frameworks/StringFormat.h>

#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"
#include "nana/gui/filebox.hpp"
#include "Renderables/MeshPrimitive.h"
#include "ViewerQueries.h"
#include "Frameworks/QueryDispatcher.h"

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

void ModelInfoPanel::initialize(MainForm* main_form)
{
    m_main = main_form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<modelnameprompt weight=28><modelname weight=28><meshnodetree>");
    m_modelNamePrompt = menew nana::label{ *this, "Model Name" };
    UISchemeColors::ApplySchemaColors(m_modelNamePrompt->scheme());
    m_modelNameLabel = menew nana::label{ *this, "xxx" };
    UISchemeColors::ApplySchemaColors(m_modelNameLabel->scheme());
    m_meshNodeTree = menew nana::treebox{ *this };
    m_meshNodeTree->events().mouse_down([this](const nana::arg_mouse& arg) { this->onMeshNodeTreeMouseDown(arg); });
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
    particleMenu->append("Cloud Particle", [this](auto item) { this->onAddCloudParticle(item); });
    particleMenu->append("Super Spray Particle", [this](auto item) { this->onAddSuperSprayParticle(item); });
    nana::menu* avatarMenu = m_popupMenu->create_sub_menu(2);
    avatarMenu->append("Change Texture", [this](auto item) { this->onChangeMeshTexture(item); });
    //m_meshNodeTree->events().mouse_down(nana::menu_popuper(*m_popupParticleMenu));
}

void ModelInfoPanel::setModelFileName(const std::string& filename)
{
    if (m_modelNameLabel) m_modelNameLabel->caption(filename);
}

void ModelInfoPanel::enumModelMeshNode(const std::shared_ptr<Enigma::Renderables::ModelPrimitive>& model)
{
    m_meshNodeTree->clear();
    if (!model) return;
    if (model->getMeshNodeTree().getMeshNodeCount() == 0) return;

    std::unordered_map<std::string, nana::treebox::item_proxy> mesh_pos_map;

    unsigned int total_node_count = model->getMeshNodeTree().getMeshNodeCount();
    unsigned int total_mesh_count = model->getMeshPrimitiveCount();
    unsigned int mesh_count = 0;

    nana::treebox::item_proxy root_pos = m_meshNodeTree->insert("root", model->getName());
    mesh_pos_map.emplace("root", root_pos);
    for (unsigned int ni = 0; ni < total_node_count; ni++)
    {
        auto mesh_node = model->getMeshNodeTree().getMeshNode(ni);
        if (!mesh_node) continue;
        nana::treebox::item_proxy parent_pos;
        if (auto parent_index = mesh_node.value().get().getParentIndexInArray())
        {
            auto parent_mesh_node = model->getMeshNodeTree().getMeshNode(parent_index.value());
            if (parent_mesh_node)
            {
                parent_pos = mesh_pos_map.find(parent_mesh_node.value().get().getName())->second;
            }
            else
            {
                parent_pos = mesh_pos_map.find("root")->second;
            }
        }
        else
        {
            parent_pos = mesh_pos_map.find("root")->second;
        }
        nana::treebox::item_proxy node_pos = parent_pos.append(mesh_node.value().get().getName(), mesh_node.value().get().getName());
        //nana::treebox::item_proxy node_pos = m_meshNodeTree->insert(
          //  parent_pos, mesh_node.value().get().getName(), mesh_node.value().get().getName());
        mesh_pos_map.emplace(mesh_node.value().get().getName(), node_pos);
        if (mesh_node.value().get().getMeshPrimitive())
        {
            node_pos.append(mesh_node.value().get().getMeshPrimitive()->id().name(), mesh_node.value().get().getMeshPrimitive()->id().name(), mesh_node.value().get().getMeshPrimitive()->id());
            //m_meshNodeTree->insert(node_pos,
              //  mesh_node.value().get().getMeshPrimitive()->getName(), mesh_node.value().get().getMeshPrimitive()->getName());
        }
    }
}

void ModelInfoPanel::onMeshNodeTreeMouseDown(const nana::arg_mouse& arg)
{
    if (!arg.right_button) return;
    if (m_meshNodeTree->selected().empty()) return;
    auto poper = menu_popuper(*m_popupMenu);
    poper(arg);
}

void ModelInfoPanel::onAddCloudParticle(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    //m_main->GetAppDelegate()->AddCloudParticleToMeshNode(m_meshNodeTree->selected().key());
}

void ModelInfoPanel::onAddSuperSprayParticle(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    //m_main->GetAppDelegate()->AddSprayParticleToMeshNode(m_meshNodeTree->selected().key());
}

void ModelInfoPanel::onChangeMeshTexture(const nana::menu::item_proxy& menu_item)
{
    if (m_meshNodeTree->selected().empty()) return;
    Enigma::Primitives::PrimitiveId mesh_id = m_meshNodeTree->selected().value<Enigma::Primitives::PrimitiveId>();
    nana::filebox fb{ *this, true };
    fb.add_filter({ {"Image File(*.png)", "*.png"} });
    auto paths = fb.show();
    if (paths.size() > 0)
    {
        std::string stem = paths[0].stem().generic_string();
        std::string sub_path = (--(--paths[0].end()))->generic_string();
        auto texure_id = std::make_shared<ResolveTextureId>(sub_path + "/" + stem)->dispatch();
        if (texure_id.name().empty()) return;
        Enigma::Frameworks::CommandBus::enqueue(std::make_shared<ChangeMeshTexture>(mesh_id, texure_id));
    }
    //m_main->GetAppDelegate()->ChangeMeshTexture(m_meshNodeTree->selected().key());
}

void ModelInfoPanel::subscribeHandlers()
{
    m_refreshModelNodeTree = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](auto c) { refreshModelNodeTree(c); });
    Enigma::Frameworks::CommandBus::subscribe(typeid(RefreshModelNodeTree), m_refreshModelNodeTree);
}

void ModelInfoPanel::unsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::unsubscribe(typeid(RefreshModelNodeTree), m_refreshModelNodeTree);
    m_refreshModelNodeTree = nullptr;
}

void ModelInfoPanel::refreshModelNodeTree(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RefreshModelNodeTree, Enigma::Frameworks::ICommand>(c);
    if (!cmd) return;
    auto model_id = cmd->model()->id();
    auto id_text = string_format("%s/%d/%s", model_id.name().c_str(), model_id.sequence(), model_id.rtti().getName().c_str());
    setModelFileName(id_text);
    enumModelMeshNode(cmd->model());
}
