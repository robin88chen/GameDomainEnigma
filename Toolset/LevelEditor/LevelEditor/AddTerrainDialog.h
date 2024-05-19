/*********************************************************************
 * \file   AddTerrainDialog.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef ADD_TERRAIN_DIALOG_H
#define ADD_TERRAIN_DIALOG_H

#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include "TerrainEditService.h"
#include "GameEngine/TextureId.h"
#include <array>

namespace LevelEditor
{
    class TerrainEditConsole;

    class AddTerrainDialog : public nana::form
    {
    public:
        AddTerrainDialog(nana::window owner, const std::shared_ptr<TerrainEditConsole>& terrain_edit, const std::string& data_path_id);
        virtual ~AddTerrainDialog();

        void onOkButton(const nana::arg_click& arg);
        void onCancelButton(const nana::arg_click& arg);
        void onTextureLayerButton(const nana::arg_click& arg);

        void onCellNumCellPerUVComboTextChanged(const nana::arg_combox& ev);

    private:
        nana::button* m_okButton;
        nana::button* m_cancelButton;

        nana::label* m_terrainNamePrompt;
        nana::textbox* m_terrainNameInputBox;

        nana::label* m_cellColumnsPrompt;
        nana::combox* m_cellColumnsCombo;
        nana::label* m_cellRowsPrompt;
        nana::combox* m_cellRowsCombo;

        nana::label* m_terrainVertexMinPrompt;
        nana::textbox* m_terrainVertexMinInputBox;
        nana::label* m_terrainVertexMaxPrompt;
        nana::textbox* m_terrainVertexMaxInputBox;

        nana::label* m_terrainUVMinPrompt;
        nana::textbox* m_terrainUVMinInputBox;
        nana::label* m_terrainUVMaxPrompt;
        nana::textbox* m_terrainUVMaxInputBox;

        nana::label* m_terrainLocalPosPrompt;
        nana::textbox* m_terrainLocalPosInputBox;

        nana::label* m_cellPerUVPrompt;
        nana::combox* m_cellPerUVCombo;

        std::array<nana::button*, TerrainEditService::TextureLayerNum> m_textureLayerButtons;
        std::array<Enigma::Engine::TextureId, TerrainEditService::TextureLayerNum> m_layerTextureIds;
        //nana::textbox* m_rootNodePrompt;
        //nana::textbox* m_rootNodeNameInputBox;

        std::weak_ptr<TerrainEditConsole> m_terrainEdit;
        std::string m_dataPathId;
    };
}


#endif // ADD_TERRAIN_DIALOG_H
