/*********************************************************************
 * \file   MainForm.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef MAIN_FORM_H
#define MAIN_FORM_H

#include "Frameworks/EventSubscriber.h"
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
    class PawnListPanel;
    class ModelListPanel;

    class MainForm : public nana::form
    {
    public:
        MainForm();
        virtual ~MainForm();

        void initialize();
        ViewerAppDelegate* appDelegate() { return m_appDelegate; }
    private:
        void initMenu();

        void onCloseCommand(const nana::menu::item_proxy& menu_item);
        void onImportDaeFile(const nana::menu::item_proxy& menu_item);
        void onCreateNewAnimatedPawn(const nana::menu::item_proxy& menu_item);
        //void onSaveAnimatedPawn(const nana::menu::item_proxy& menu_item);
        //void onLoadAnimatedPawn(const nana::menu::item_proxy& menu_item);

        void finalize();

        void onViewingPawnConstituted(const Enigma::Frameworks::IEventPtr& e);
        void onConstituteViewingPawnFailed(const Enigma::Frameworks::IEventPtr& e);

    private:
        Enigma::Frameworks::EventSubscriberPtr m_onViewingPawnConstituted;
        Enigma::Frameworks::EventSubscriberPtr m_onConstituteViewingPawnFailed;

        inline static std::string m_appCaption = "Enigma Viewer";

        nana::menubar* m_menubar;
        nana::tabbar<int>* m_entitiesTabbar;
        nana::tabbar<int>* m_toolsTabbar;
        ViewerAppDelegate* m_appDelegate;
        nana::timer* m_timer;
        RenderPanel* m_renderPanel;
        OutputPanel* m_outputPanel;
        ModelInfoPanel* m_modelInfoPanel;
        AnimationInfoPanel* m_animationInfoPanel;
        PawnListPanel* m_pawnListPanel;
        ModelListPanel* m_modelListPanel;
    };
}


#endif // MAIN_FORM_H
