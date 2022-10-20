/*********************************************************************
 * \file   ListboxInlineTextbox.h
 * \brief  inline list box item
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _LISTBOX_INLINE_TEXTBOX_H
#define _LISTBOX_INLINE_TEXTBOX_H

#include "nana/gui/widgets/listbox.hpp"
#include "nana/gui/widgets/textbox.hpp"

namespace EnigmaViewer
{
    class InlineTextBoxItem : public nana::listbox::inline_notifier_interface
    {
    public:
        /// A message to create the inline widget
        virtual void create(nana::window wd) override;

        /// A message to activate the inline widget to attach a specified item
        virtual void activate(inline_indicator& ind, index_type pos) override;

        /// A message to change the status
        virtual void notify_status(status_type status, bool status_on) override;

        /// A message to resize the inline widget
        virtual void resize(const nana::size& dimension) override;

        /// A message to set the value from the item
        virtual void set(const value_type& value) override;

        /// Determines whether to draw the background of the widget
        virtual bool whether_to_draw() const override;

    private:
        inline_indicator* m_indicator{ nullptr };
        index_type m_pos;
        nana::textbox m_txt;
        std::string m_prevText;
    };
}
#endif // !_LISTBOX_INLINE_TEXTBOX_H
