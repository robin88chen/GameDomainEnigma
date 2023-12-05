#include "AnimationInfoPanel.h"
#include "AnimationClipInfoItem.h"
#include "SchemeColorDef.h"
#include "ListboxInlineTextbox.h"
#include "ListboxInlineCombox.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/StringFormat.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "ViewerCommands.h"
#include "GameCommon/AnimationClipMapEvents.h"
#include "ViewerEvents.h"

using namespace EnigmaViewer;
using namespace Enigma::Frameworks;

AnimationInfoPanel::AnimationInfoPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_tableGroup = nullptr;
    m_actionTableBox = nullptr;
    m_addActionButton = nullptr;
    m_deleteActionButton = nullptr;
    m_loadActionButton = nullptr;
    m_saveActionButton = nullptr;

    m_actionPrompt = nullptr;
    m_actionCombox = nullptr;
    m_speedPrompt = nullptr;
    m_speedSlider = nullptr;
}

AnimationInfoPanel::~AnimationInfoPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_tableGroup);
    SAFE_DELETE(m_actionTableBox);
    SAFE_DELETE(m_addActionButton);
    SAFE_DELETE(m_deleteActionButton);
    SAFE_DELETE(m_loadActionButton);
    SAFE_DELETE(m_saveActionButton);

    SAFE_DELETE(m_actionPrompt);
    SAFE_DELETE(m_actionCombox);
    SAFE_DELETE(m_speedPrompt);
    SAFE_DELETE(m_speedSlider);
}

void AnimationInfoPanel::Initialize()
{
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<table_group><row_play_action weight=28><row_speed weight=28>");
    //m_tableGroup = menew nana::group{ *this , "<color=0xf1f1f1>Action Table</>", true };
    m_tableGroup = menew nana::group{ *this };// , "Action Table"}; // , true};
    m_tableGroup->caption("<color=0xf1f1f1>Action Table</>");
    m_tableGroup->enable_format_caption(true);

    m_tableGroup->div("margin=5 vert<row_btn weight=28><row_table><row_btn2 weight=28>");
    (*m_place)["table_group"] << *m_tableGroup;
    m_addActionButton = menew nana::button{ *m_tableGroup, "Add" };
    m_addActionButton->transparent(true).enable_focus_color(false);
    UISchemeColors::ApplySchemaColors(m_addActionButton->scheme());
    m_addActionButton->events().click([this](const nana::arg_click& a) { this->OnAddActionButton(a); });
    m_deleteActionButton = menew nana::button{ *m_tableGroup, "Delete" };
    m_deleteActionButton->events().click([this](const nana::arg_click& a) { this->OnDeleteActionButton(a); });
    m_deleteActionButton->transparent(true).enable_focus_color(false);
    UISchemeColors::ApplySchemaColors(m_deleteActionButton->scheme());
    (*m_tableGroup)["row_btn"] << *m_addActionButton << *m_deleteActionButton;

    m_actionTableBox = menew nana::listbox{ *m_tableGroup };
    UISchemeColors::ApplySchemaColors(m_actionTableBox->scheme());
    m_actionTableBox->scheme().header_bgcolor = UISchemeColors::BACKGROUND;
    m_actionTableBox->scheme().header_fgcolor = UISchemeColors::FOREGROUND;
    m_actionTableBox->scheme().item_highlighted = UISchemeColors::HIGHLIGHT_BG;
    m_actionTableBox->scheme().item_selected = UISchemeColors::SELECT_BG;
    m_actionTableBox->append_header("Name", 60);
    m_actionTableBox->append_header("Start", 60);
    m_actionTableBox->append_header("Loop", 60);
    m_actionTableBox->append_header("Divide", 60);
    m_actionTableBox->append_header("Wrap", 60);
    m_actionTableBox->at(0).inline_factory(0, nana::pat::make_factory<InlineTextBoxItem>());
    m_actionTableBox->at(0).inline_factory(1, nana::pat::make_factory<InlineTextBoxItem>());
    m_actionTableBox->at(0).inline_factory(2, nana::pat::make_factory<InlineTextBoxItem>());
    m_actionTableBox->at(0).inline_factory(3, nana::pat::make_factory<InlineTextBoxItem>());
    m_actionTableBox->at(0).inline_factory(4, nana::pat::make_factory<InlineComboxItem>());

    //m_actionTableBox->events().selected([this](const nana::arg_listbox& ar) { this->OnActionItemSelected(ar); });
    //m_actionTableBox->events().dbl_click([this](const nana::arg_mouse& ar) { this->OnItemDblClicked(ar); });
    (*m_tableGroup)["row_table"] << *m_actionTableBox;

    m_loadActionButton = menew nana::button{ *m_tableGroup, "Load" };
    m_loadActionButton->transparent(true).enable_focus_color(false);
    UISchemeColors::ApplySchemaColors(m_loadActionButton->scheme());
    //m_browseButton->events().click([=](const nana::arg_click& a)
      //  { this->OnBrowseRootButton(a); });  // a 的 type 用 auto 會編譯失敗， why??
    m_saveActionButton = menew nana::button{ *m_tableGroup, "Save" };
    m_saveActionButton->transparent(true).enable_focus_color(false);
    UISchemeColors::ApplySchemaColors(m_saveActionButton->scheme());
    (*m_tableGroup)["row_btn2"] << *m_loadActionButton << *m_saveActionButton;

    m_actionPrompt = menew nana::label{ *this, "Play" };
    UISchemeColors::ApplySchemaColors(m_actionPrompt->scheme());
    m_actionCombox = menew nana::combox{ *this };
    m_actionCombox->events().text_changed([this](const nana::arg_combox& a) { this->OnActionComboTextChanged(a); });
    // background 沒有作用，所以顏色都不要變了
    //UISchemeColors::ApplySchemaColors(m_actionCombox->scheme());
    (*m_actionCombox).push_back("one").push_back("two").push_back("three");
    (*m_place)["row_play_action"] << *m_actionPrompt << *m_actionCombox;

    m_speedPrompt = menew nana::label{ *this, "Speed" };
    UISchemeColors::ApplySchemaColors(m_speedPrompt->scheme());
    m_speedSlider = menew nana::slider{ *this };
    UISchemeColors::ApplySchemaColors(m_speedSlider->scheme());
    m_speedSlider->maximum(100);
    m_speedSlider->vernier([](unsigned int maximum, unsigned int value)
        { return string_format("%4.2f", (float)value / 10.0f); });
    (*m_place)["row_speed"] << *m_speedPrompt << *m_speedSlider;

    m_place->collocate();
}

void AnimationInfoPanel::OnAddActionButton(const nana::arg_click& ev)
{
    if (!m_actionTableBox) return;
    auto cat = m_actionTableBox->at(0); //Get the proxy to the default category
    unsigned int count = cat.size();
    std::string name = string_format("Item %d", count);
    AnimClipInfoItem clip{ name };
    cat.append(clip);
    Enigma::Frameworks::CommandBus::post(std::make_shared<AddAnimationClip>(clip.m_name, clip.m_clip));
}

void AnimationInfoPanel::OnDeleteActionButton(const nana::arg_click& ev)
{
    if (!m_actionTableBox) return;
    auto idx_pairs = m_actionTableBox->selected();
    if (idx_pairs.empty()) return;
    for (auto idx : idx_pairs)
    {
        auto item = m_actionTableBox->at(idx.cat).at(idx.item);
        AnimClipInfoItem clip{ "" };
        item->resolve_to(clip);
        Enigma::Frameworks::CommandBus::post(std::make_shared<DeleteAnimationClip>(clip.m_name));
    }
    m_actionTableBox->erase(idx_pairs);
}

void AnimationInfoPanel::OnActionComboTextChanged(const nana::arg_combox& ev)
{
    Enigma::Frameworks::CommandBus::post(std::make_shared<PlayAnimationClip>(ev.widget.caption()));
}

void AnimationInfoPanel::SubscribeHandlers()
{
    m_onAnimationClipMapChanged = std::make_shared<EventSubscriber>([=](auto e) { OnAnimationClipMapChanged(e); });
    EventPublisher::subscribe(typeid(Enigma::GameCommon::AnimationClipMapChanged), m_onAnimationClipMapChanged);
    m_onAnimationClipItemUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnAnimationClipItemUpdated(e); });
    EventPublisher::subscribe(typeid(AnimationClipItemUpdated), m_onAnimationClipItemUpdated);

    m_doRefreshingAnimClipMap = std::make_shared<CommandSubscriber>([=](auto c) { DoRefreshingAnimClipMap(c); });
    CommandBus::subscribe(typeid(RefreshAnimationClipList), m_doRefreshingAnimClipMap);
}

void AnimationInfoPanel::UnsubscribeHandlers()
{
    EventPublisher::unsubscribe(typeid(Enigma::GameCommon::AnimationClipMapChanged), m_onAnimationClipMapChanged);
    m_onAnimationClipMapChanged = nullptr;
    EventPublisher::unsubscribe(typeid(AnimationClipItemUpdated), m_onAnimationClipItemUpdated);
    m_onAnimationClipItemUpdated = nullptr;

    CommandBus::unsubscribe(typeid(RefreshAnimationClipList), m_doRefreshingAnimClipMap);
    m_doRefreshingAnimClipMap = nullptr;
}

void AnimationInfoPanel::RefreshActionCombo()
{
}

void AnimationInfoPanel::OnAnimationClipMapChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::GameCommon::AnimationClipMapChanged, Enigma::Frameworks::IEvent>(e);
    if (!ev) return;
    m_actionCombox->clear();
    for (auto clip : ev->GetClipMap())
    {
        m_actionCombox->push_back(clip.second.getName());
    }
}

void AnimationInfoPanel::OnAnimationClipItemUpdated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<AnimationClipItemUpdated, IEvent>(e);
    if (!ev) return;
    auto cat = m_actionTableBox->at(ev->GetCategoryIndex());
    auto item = cat.at(ev->GetItemIndex());
    AnimClipInfoItem clip{ "" };
    item->resolve_to(clip);
    CommandBus::post(std::make_shared<ChangeAnimationTimeValue>(ev->GetOldText(), clip.m_name, clip.m_clip));
}

void AnimationInfoPanel::DoRefreshingAnimClipMap(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RefreshAnimationClipList, Enigma::Frameworks::ICommand>(c);
    if (!cmd) return;
    m_actionCombox->clear();
    auto cat = m_actionTableBox->at(0); //Get the proxy to the default category
    for (auto clip : cmd->GetClipMap().GetAnimationClipMap())
    {
        AnimClipInfoItem clip_info{ clip.second.getName() };
        clip_info.m_clip = clip.second.GetClip();
        cat.append(clip_info);
        m_actionCombox->push_back(clip.second.getName());
    }
}
