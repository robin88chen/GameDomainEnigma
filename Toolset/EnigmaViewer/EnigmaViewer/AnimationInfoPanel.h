/*********************************************************************
 * \file   AnimationInfoPanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _ANIMATION_INFO_PANEL_H
#define _ANIMATION_INFO_PANEL_H

#include "Frameworks/CommandSubscriber.h"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/listbox.hpp"
#include "nana/gui/widgets/group.hpp"
#include "nana/gui/widgets/combox.hpp"
#include "nana/gui/widgets/slider.hpp"
#include "Frameworks/EventSubscriber.h"

namespace EnigmaViewer
{
    class ViewerAppDelegate;

    class AnimationInfoPanel : public nana::panel<false>
    {
    public:
        AnimationInfoPanel(const nana::window& wd);
        virtual ~AnimationInfoPanel();

        void initialize();
        void onAddActionButton(const nana::arg_click& ev);
        void onDeleteActionButton(const nana::arg_click& ev);

        void onActionComboTextChanged(const nana::arg_combox& ev);

        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void onAnimationClipMapChanged(const Enigma::Frameworks::IEventPtr& e);
        void onAnimationClipItemUpdated(const Enigma::Frameworks::IEventPtr& e);

        void refreshAnimClipMap(const Enigma::Frameworks::ICommandPtr& c);

        void refreshActionCombo();
    private:
        nana::place* m_place;
        nana::group* m_tableGroup;
        nana::listbox* m_actionTableBox;
        nana::button* m_addActionButton;
        nana::button* m_deleteActionButton;
        nana::button* m_loadActionButton;
        nana::button* m_saveActionButton;
        nana::label* m_actionPrompt;
        nana::combox* m_actionCombox;
        nana::label* m_speedPrompt;
        nana::slider* m_speedSlider;

        Enigma::Frameworks::EventSubscriberPtr m_onAnimationClipMapChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onAnimationClipItemUpdated;

        Enigma::Frameworks::CommandSubscriberPtr m_refreshAnimClipMap;
    };
}


#endif // _ANIMATION_INFO_PANEL_H
