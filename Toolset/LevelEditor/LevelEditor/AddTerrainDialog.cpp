#include "AddTerrainDialog.h"
#include "nana/gui/filebox.hpp"
#include "Platforms/MemoryMacro.h"
#include "EditorUtilities.h"
#include "Frameworks/CommandBus.h"
#include "LevelEditorCommands.h"
#include "Frameworks/StringFormat.h"
#include "WorldEditConsole.h"
#include <cstdlib>
#include <MathLib/Matrix4.h>

using namespace LevelEditor;
using namespace Enigma;

std::vector<std::string> cellNumCandidates =
{
    "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
};
std::vector<std::string> cellPerUVCandidates =
{
    "1", "2", "4", "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
};

#define PRESET_MIN_VERTEX   "-1024.0, 0.0, -1024.0"
#define PRESET_MAX_VERTEX   "1024.0, 0.0, 1024.0"
#define PRESET_MIN_UV   "0.0, 0.0"
#define PRESET_MAX_UV   "1.0, 1.0"
#define PRESET_LOCAL_POS "0.0, 0.0, 0.0"

AddTerrainDialog::AddTerrainDialog(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_edit, const std::string& media_path_id) : form(owner, nana::API::make_center(400, 600), nana::appear::decorate<>{})
{
    m_worldEdit = world_edit;
    m_mediaPathId = media_path_id;

    caption("Add New Terrain");
    get_place().div("vert<><create_prompt arrange=[40%,variable] margin=[10,20]><cell_prompt margin=[10,20]><min_vtx_prompt margin=[10,20]><max_vtx_prompt margin=[10,20]><min_uv_prompt margin=[10,20]><max_uv_prompt margin=[10,20]><local_pos_prompt margin=[10,20]><cell_uv_prompt margin=[10,20]><texture_btns weight=84 arrange=[64,64,64,64] margin=[10,20] gap=10><><buttons margin=[10,40] gap=10><>");
    m_terrainNamePrompt = menew nana::label(*this, "Terrain Name : ");
    (*m_terrainNamePrompt).text_align(nana::align::right);
    m_terrainNameInputBox = new nana::textbox(*this, "Name");
    get_place()["create_prompt"] << *m_terrainNamePrompt << *m_terrainNameInputBox;

    m_cellColumnsPrompt = menew nana::label(*this, "Cell Columns : ");
    (*m_cellColumnsPrompt).text_align(nana::align::right);
    m_cellColumnsCombo = menew nana::combox{ *this };
    m_cellColumnsCombo->events().text_changed([this](const nana::arg_combox& a) { this->onCellNumCellPerUVComboTextChanged(a); });
    m_cellRowsPrompt = menew nana::label(*this, "Cell Rows : ");
    (*m_cellRowsPrompt).text_align(nana::align::right);
    m_cellRowsCombo = menew nana::combox{ *this };
    m_cellRowsCombo->events().text_changed([this](const nana::arg_combox& a) { this->onCellNumCellPerUVComboTextChanged(a); });
    for (auto cell_num_candidate : cellNumCandidates)
    {
        m_cellColumnsCombo->push_back(cell_num_candidate);
        m_cellRowsCombo->push_back(cell_num_candidate);
    }
    get_place()["cell_prompt"] << *m_cellColumnsPrompt << *m_cellColumnsCombo << *m_cellRowsPrompt << *m_cellRowsCombo;

    m_terrainVertexMinPrompt = menew nana::label(*this, "Terrain Min Vertex Pos : ");
    (*m_terrainVertexMinPrompt).text_align(nana::align::right);
    m_terrainVertexMinInputBox = new nana::textbox(*this, PRESET_MIN_VERTEX);
    m_terrainVertexMaxPrompt = menew nana::label(*this, "Terrain Max Vertex Pos : ");
    (*m_terrainVertexMaxPrompt).text_align(nana::align::right);
    m_terrainVertexMaxInputBox = new nana::textbox(*this, PRESET_MAX_VERTEX);
    get_place()["min_vtx_prompt"] << *m_terrainVertexMinPrompt << *m_terrainVertexMinInputBox;
    get_place()["max_vtx_prompt"] << *m_terrainVertexMaxPrompt << *m_terrainVertexMaxInputBox;

    m_terrainUVMinPrompt = menew nana::label(*this, "Terrain Min UV : ");
    (*m_terrainUVMinPrompt).text_align(nana::align::right);
    m_terrainUVMinInputBox = new nana::textbox(*this, PRESET_MIN_UV);
    m_terrainUVMaxPrompt = menew nana::label(*this, "Terrain Max UV : ");
    (*m_terrainUVMaxPrompt).text_align(nana::align::right);
    m_terrainUVMaxInputBox = new nana::textbox(*this, PRESET_MAX_UV);
    get_place()["min_uv_prompt"] << *m_terrainUVMinPrompt << *m_terrainUVMinInputBox;
    get_place()["max_uv_prompt"] << *m_terrainUVMaxPrompt << *m_terrainUVMaxInputBox;

    m_terrainLocalPosPrompt = menew nana::label(*this, "Terrain Local Pos : ");
    (*m_terrainLocalPosPrompt).text_align(nana::align::right);
    m_terrainLocalPosInputBox = new nana::textbox(*this, PRESET_LOCAL_POS);
    get_place()["local_pos_prompt"] << *m_terrainLocalPosPrompt << *m_terrainLocalPosInputBox;

    m_cellPerUVPrompt = menew nana::label(*this, "Cell Per UV Units : ");
    (*m_cellPerUVPrompt).text_align(nana::align::right);
    m_cellPerUVCombo = menew nana::combox{ *this };
    m_cellPerUVCombo->events().text_changed([this](const nana::arg_combox& a) { this->onCellNumCellPerUVComboTextChanged(a); });
    for (auto cell_uv_candidate : cellPerUVCandidates)
    {
        m_cellPerUVCombo->push_back(cell_uv_candidate);
    }
    get_place()["cell_uv_prompt"] << *m_cellPerUVPrompt << *m_cellPerUVCombo;

    for (auto& btn : m_textureLayerButtons)
    {
        btn = menew nana::button(*this, nana::rectangle(nana::size(64, 64)));
        btn->events().click([this](const nana::arg_click& a) { this->onTextureLayerButton(a); });
        get_place()["texture_btns"] << *btn;
    }

    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

AddTerrainDialog::~AddTerrainDialog()
{
    SAFE_DELETE(m_terrainNamePrompt);
    SAFE_DELETE(m_terrainNameInputBox);
    SAFE_DELETE(m_cellColumnsPrompt);
    SAFE_DELETE(m_cellColumnsCombo);
    SAFE_DELETE(m_cellRowsPrompt);
    SAFE_DELETE(m_cellRowsCombo);

    SAFE_DELETE(m_terrainVertexMinPrompt);
    SAFE_DELETE(m_terrainVertexMinInputBox);
    SAFE_DELETE(m_terrainVertexMaxPrompt);
    SAFE_DELETE(m_terrainVertexMaxInputBox);

    SAFE_DELETE(m_terrainUVMinPrompt);
    SAFE_DELETE(m_terrainUVMinInputBox);
    SAFE_DELETE(m_terrainUVMaxPrompt);
    SAFE_DELETE(m_terrainUVMaxInputBox);

    SAFE_DELETE(m_terrainLocalPosPrompt);
    SAFE_DELETE(m_terrainLocalPosInputBox);

    SAFE_DELETE(m_cellPerUVPrompt);
    SAFE_DELETE(m_cellPerUVCombo);

    for (auto& btn : m_textureLayerButtons)
    {
        SAFE_DELETE(btn);
    }
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
}

void AddTerrainDialog::onOkButton(const nana::arg_click& arg)
{
    std::string terrainName = m_terrainNameInputBox->text();
    //terrainName = m_worldEdit.lock()->getCurrentWorldFolder() + "/" + terrainName;
    char* endptr = nullptr;
    unsigned rows, cols;
    rows = std::strtol(m_cellRowsCombo->caption().c_str(), &endptr, 10);
    cols = std::strtol(m_cellColumnsCombo->caption().c_str(), &endptr, 10);
    if ((rows == 0) || (cols == 0)) return;
    bool isParseOk;
    MathLib::Vector3 minVertexPos, maxVertexPos;
    MathLib::Vector2 minUV, maxUV;
    if (std::tie(minVertexPos, isParseOk) = parseTextToVector3(m_terrainVertexMinInputBox->text()); !isParseOk) return;
    if (std::tie(maxVertexPos, isParseOk) = parseTextToVector3(m_terrainVertexMaxInputBox->text()); !isParseOk) return;
    if (std::tie(minUV, isParseOk) = parseTextToVector2(m_terrainUVMinInputBox->text()); !isParseOk) return;
    if (std::tie(maxUV, isParseOk) = parseTextToVector2(m_terrainUVMaxInputBox->text()); !isParseOk) return;
    MathLib::Vector3 terrainLocalPos;
    if (std::tie(terrainLocalPos, isParseOk) = parseTextToVector3(m_terrainLocalPosInputBox->text()); !isParseOk) return;

    /*Terrain::TerrainGeometryDtoHelper terrain_helper(terrainName);
    terrain_helper.NumRows(rows).NumCols(cols).MinPosition(minVertexPos).MaxPosition(maxVertexPos).MinTextureCoordinate(minUV).MinTextureCoordinate(maxUV);

    Frameworks::CommandBus::post(std::make_shared<CreateNewTerrain>(terrainName, terrain_helper.ToDto(), m_layerTextureFilenames, terrainLocalPos, m_worldEdit.lock()->getWorldMapPathId()));
    */
    close();
}

void AddTerrainDialog::onCancelButton(const nana::arg_click& arg)
{
    close();
}

void AddTerrainDialog::onTextureLayerButton(const nana::arg_click& arg)
{
    int btn_idx = -1;
    for (int i = 0; i < (int)m_textureLayerButtons.size(); i++)
    {
        if (m_textureLayerButtons[i]->handle() == arg.window_handle)
        {
            btn_idx = i;
            break;
        }
    }
    if (btn_idx < 0) return;
    //nana::button* btn = dynamic_cast<nana::button*>(nana::API::get_widget(arg.window_handle));
    nana::filebox fb(handle(), true);
    fb.add_filter({ {"PNG File(*.png)", "*.png"},{"Bitmap File(*.bmp)", "*.bmp"} }).title("Select Layer Texture");
    if (auto paths = fb.show(); !paths.empty())
    {
        pasteTextureImageToButton(paths[0].string(), m_textureLayerButtons[btn_idx], 64);
        std::string path_string = filePathCombinePathID(paths[0], m_mediaPathId);
        m_layerTextureFilenames[btn_idx] = path_string;
    }
}

void AddTerrainDialog::onCellNumCellPerUVComboTextChanged(const nana::arg_combox& ev)
{
    char* endptr = nullptr;
    int cellPerUV = std::strtol(m_cellPerUVCombo->caption().c_str(), &endptr, 10);
    if (cellPerUV <= 0) return;
    int cellNumX = std::strtol(m_cellColumnsCombo->caption().c_str(), &endptr, 10);
    if (cellNumX <= 0) return;
    int cellNumZ = std::strtol(m_cellRowsCombo->caption().c_str(), &endptr, 10);
    if (cellNumZ <= 0) return;
    float addressU = (float)(cellNumX) / (float)(cellPerUV);
    float addressV = (float)(cellNumZ) / (float)(cellPerUV);
    std::string uvText = string_format("%8.3f, %8.3f", addressU, addressV);
    m_terrainUVMaxInputBox->caption(uvText);
}

