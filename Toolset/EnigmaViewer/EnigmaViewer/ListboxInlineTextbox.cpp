#include "ListboxInlineTextbox.h"
#include "Frameworks/EventPublisher.h"
#include "SchemeColorDef.h"
#include "ViewerEvents.h"

void EnigmaViewer::InlineTextBoxItem::create(nana::window wd)
{
    m_txt.create(wd);
    UISchemeColors::ApplySchemaColors(m_txt.scheme());
    m_txt.line_wrapped(false).multi_lines(false);
    m_txt.events().click([this]
        {
            //当点击文本框时，选中本项
            m_indicator->selected(m_pos);
        });

    m_txt.events().mouse_move([this]
        {
            //当鼠标在文本框上移动时，需要高亮本项
            m_indicator->hovered(m_pos);
        });

    m_txt.events().key_char([this](const nana::arg_keyboard& arg)
        {
            if (arg.key == nana::keyboard::enter)
            {
                //在文本框里按下回车，则修改本项的值
                arg.ignore = true;
                m_indicator->modify(m_pos, m_txt.caption());
                Enigma::Frameworks::EventPublisher::Post(std::make_shared<AnimationClipItemUpdated>(static_cast<unsigned int>(m_pos.cat), static_cast<unsigned int>(m_pos.item), m_prevText, m_txt.caption()));
                m_prevText = m_txt.caption();
            }
        });
}

void EnigmaViewer::InlineTextBoxItem::activate(inline_indicator& ind, index_type pos)
{
    m_indicator = &ind;
    m_pos = pos;
}

void EnigmaViewer::InlineTextBoxItem::notify_status(status_type status, bool status_on)
{
    //Sets focus for the textbox when the item is selected
    if ((status_type::selecting == status) && status_on)
        m_txt.focus();
}

void EnigmaViewer::InlineTextBoxItem::resize(const nana::size& dimension)
{
    m_txt.size(dimension);
}

void EnigmaViewer::InlineTextBoxItem::set(const value_type& value)
{
    if (m_prevText.empty()) m_prevText = value;
    m_txt.caption(value);
}

bool EnigmaViewer::InlineTextBoxItem::whether_to_draw() const
{
    return true;
}
