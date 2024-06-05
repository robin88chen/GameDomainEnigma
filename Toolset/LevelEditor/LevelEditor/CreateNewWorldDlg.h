/*********************************************************************
 * \file   CreateNewWorldDlg.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef CREATE_NEW_WORLD_DLG_H
#define CREATE_NEW_WORLD_DLG_H

#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include "SceneGraph/SpatialId.h"

namespace LevelEditor
{
    class WorldEditConsole;
    class CreateNewWorldDlg : public nana::form
    {
    public:
        CreateNewWorldDlg(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_editor, const Enigma::SceneGraph::SpatialId& portal_manager_id);
        virtual ~CreateNewWorldDlg();

    private:
        void onOkButton(const nana::arg_click& arg);
        void onCancelButton(const nana::arg_click& arg);
        void onInputNameChanged(const nana::arg_textbox& arg);

        Enigma::SceneGraph::SpatialId outsideRegionFromInput();

        std::error_code validateInputNames();
    private:
        nana::button* m_okButton;
        nana::button* m_cancelButton;
        nana::label* m_namePrompt;
        nana::textbox* m_nameInputBox;
        nana::checkbox* m_regionCheckbox;
        nana::textbox* m_regionNameInputBox;
        nana::label* m_nameValidationPrompt;

        std::weak_ptr<WorldEditConsole> m_worldEditor;
        Enigma::SceneGraph::SpatialId m_portalManagerId;
    };
}

#endif // CREATE_NEW_WORLD_DLG_H
