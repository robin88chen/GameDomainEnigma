#include "OutputPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"

using namespace EnigmaViewer;

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

void OutputPanel::initialize()
{
    m_place = menew nana::place{ *this };
    m_place->div("<textbox>");
    m_outputText = menew nana::textbox{ *this };
    UISchemeColors::ApplySchemaColors(m_outputText->scheme());
    m_place->field("textbox") << *m_outputText;
}

void OutputPanel::addMessage(const std::string& msg)
{
    m_outputText->append(msg + "\n", false);
}

void OutputPanel::outputMessage(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<OutputMessage, Enigma::Frameworks::ICommand>(c);
    if (!cmd) return;
    addMessage(cmd->message());
}

void OutputPanel::subscribeHandlers()
{
    m_outputMessage = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](auto c) { outputMessage(c); });
    Enigma::Frameworks::CommandBus::subscribe(typeid(OutputMessage), m_outputMessage);
}

void OutputPanel::unsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::unsubscribe(typeid(OutputMessage), m_outputMessage);
    m_outputMessage = nullptr;
}
