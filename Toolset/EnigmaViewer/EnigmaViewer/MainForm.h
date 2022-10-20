/*********************************************************************
 * \file   MainForm.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef MAIN_FORM_H
#define MAIN_FORM_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/tabbar.hpp"
#include "nana/gui/timer.hpp"

namespace EnigmaViewer
{
    class ViewerAppDelegate;
    class RenderPanel;
    class ModelInfoPanel;
    class OutputPanel;
    class AnimationInfoPanel;

    class MainForm : public nana::form
    {
    public:
        MainForm();
        virtual ~MainForm();

        void Initialize();
        ViewerAppDelegate* GetAppDelegate() { return m_appDelegate; }
    private:
        void InitMenu();

        void OnCloseCommand(const nana::menu::item_proxy& menu_item);
        void OnImportDaeFile(const nana::menu::item_proxy& menu_item);
        void OnSaveEntity(const nana::menu::item_proxy& menu_item);
        void OnLoadEntity(const nana::menu::item_proxy& menu_item);

        void Finalize();

    private:
        nana::menubar* m_menubar;
        nana::tabbar<int>* m_tabbar;
        ViewerAppDelegate* m_appDelegate;
        nana::timer* m_timer;
        RenderPanel* m_renderPanel;
        OutputPanel* m_outputPanel;
        ModelInfoPanel* m_modelInfoPanel;
        AnimationInfoPanel* m_animationInfoPanel;
    };
}


#endif // MAIN_FORM_H
