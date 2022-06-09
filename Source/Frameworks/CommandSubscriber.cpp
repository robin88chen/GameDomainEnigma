#include "CommandSubscriber.h"

using namespace Enigma::Frameworks;

CommandSubscriber::CommandSubscriber()
{
}

CommandSubscriber::CommandSubscriber(const CommandHandler& handler) : m_handler(handler)
{
}

CommandSubscriber::~CommandSubscriber()
{
    m_handler = nullptr;
}

void CommandSubscriber::HandleCommand(const ICommandPtr& e)
{
    if (m_handler) m_handler(e);
}
