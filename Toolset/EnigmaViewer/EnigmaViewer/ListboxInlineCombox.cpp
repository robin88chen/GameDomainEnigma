#include "ListboxInlineCombox.h"
#include "SchemeColorDef.h"
#include "ViewerEvents.h"
#include "Frameworks/EventPublisher.h"

void EnigmaViewer::InlineComboxItem::create(nana::window wd)
{
    m_combo.create(wd);
    // background 沒有作用，所以顏色都不要變了
    //UISchemeColors::ApplySchemaColors(m_combo.scheme());
    //m_combo.scheme().selection = UISchemeColors::SELECT_BG;
    //m_combo.scheme().selection_unfocused = UISchemeColors::SELECT_BG;
    //m_combo.scheme().selection_text = UISchemeColors::FOREGROUND;
    m_combo.push_back("Loop").push_back("Clamp");
    m_combo.events().click([this]
        {
            //当点击文本框时，选中本项
            m_indicator->selected(m_pos);
        });

    m_combo.events().mouse_move([this]
        {
            //当鼠标在文本框上移动时，需要高亮本项
            m_indicator->hovered(m_pos);
        });
    m_combo.events().text_changed([this](const nana::arg_combox& ar)
        {
            //ar.ignore = true;
            m_indicator->modify(m_pos, m_combo.caption());
            auto s = ar.widget.caption();
            auto s1 = m_combo.text(0);
            auto s2 = m_combo.text(1);
            Enigma::Frameworks::EventPublisher::enqueue(std::make_shared<AnimationClipItemUpdated>(static_cast<unsigned int>(m_pos.cat), static_cast<unsigned int>(m_pos.item), "", ""));
        });
}

void EnigmaViewer::InlineComboxItem::activate(inline_indicator& ind, index_type pos)
{
    m_indicator = &ind;
    m_pos = pos;
}

void EnigmaViewer::InlineComboxItem::notify_status(status_type status, bool status_on)
{
    if ((status_type::selecting == status) && status_on)
        m_combo.focus();
}

void EnigmaViewer::InlineComboxItem::resize(const nana::size& dimension)
{
    m_combo.size(dimension);
}

void EnigmaViewer::InlineComboxItem::set(const value_type& value)
{
}

bool EnigmaViewer::InlineComboxItem::whether_to_draw() const
{
    return true;
}
