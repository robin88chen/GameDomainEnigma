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
#include <array>

namespace LevelEditor
{
    class WorldEditConsole;

    class AddTerrainDialog : public nana::form
    {
    public:
        AddTerrainDialog(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_edit, const std::string& media_path_id);
        virtual ~AddTerrainDialog();

        void OnOkButton(const nana::arg_click& arg);
        void OnCancelButton(const nana::arg_click& arg);
        void OnTextureLayerButton(const nana::arg_click& arg);

        void OnCellNumCellPerUVComboTextChanged(const nana::arg_combox& ev);

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
        std::array<std::string, TerrainEditService::TextureLayerNum> m_layerTextureFilenames;
        //nana::textbox* m_rootNodePrompt;
        //nana::textbox* m_rootNodeNameInputBox;

        std::weak_ptr<WorldEditConsole> m_worldEdit;
        std::string m_mediaPathId;
    };
}


#endif // ADD_TERRAIN_DIALOG_H
