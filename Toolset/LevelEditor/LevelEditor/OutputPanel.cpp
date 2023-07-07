#include "OutputPanel.h"
#include "Frameworks/Command.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"
#include "LevelEditorCommands.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/CommandBus.h"

using namespace LevelEditor;

OutputPanel::OutputPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_outputText = nullptr;
    m_place = nullptr;
}

OutputPanel::~OutputPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_outputText);
}

void OutputPanel::Initialize()
{
    m_place = menew nana::place{ *this };
    m_place->div("<textbox>");
    m_outputText = menew nana::textbox{ *this };
    UISchemeColors::ApplySchemaColors(m_outputText->scheme());
    m_place->field("textbox") << *m_outputText;

}

void OutputPanel::AddMessage(const std::string& msg)
{
    m_outputText->append(msg + "\n", false);
}

void OutputPanel::DoOutputMessage(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<OutputMessage, Enigma::Frameworks::ICommand>(c);
    if (!cmd) return;
    AddMessage(cmd->GetMessage());
}

void OutputPanel::SubscribeHandlers()
{
    m_doOutputMessage = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](auto c) { DoOutputMessage(c); });
    Enigma::Frameworks::CommandBus::Subscribe(typeid(OutputMessage), m_doOutputMessage);
}

void OutputPanel::UnsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::Unsubscribe(typeid(OutputMessage), m_doOutputMessage);
    m_doOutputMessage = nullptr;
}
