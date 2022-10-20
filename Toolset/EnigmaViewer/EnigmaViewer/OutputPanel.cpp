#include "OutputPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"

using namespace EnigmaViewer;

EnigmaViewer::OutputPanel::OutputPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_outputText = nullptr;
    m_place = nullptr;
}

EnigmaViewer::OutputPanel::~OutputPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_outputText);
}

void EnigmaViewer::OutputPanel::Initialize()
{
    m_place = menew nana::place{ *this };
    m_place->div("<textbox>");
    m_outputText = menew nana::textbox{ *this };
    UISchemeColors::ApplySchemaColors(m_outputText->scheme());
    m_place->field("textbox") << *m_outputText;
}

void EnigmaViewer::OutputPanel::AddMessage(const std::string& msg)
{
    m_outputText->append(msg + "\n", false);
}
