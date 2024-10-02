#include "CommandSubscriber.h"
#include "CommandBus.h"

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

void CommandSubscriber::handleCommand(const ICommandPtr& e)
{
    if (m_handler) m_handler(e);
}

CommandSubscriberPtr Enigma::Frameworks::commandSubscription(const std::type_info& ev_type, const CommandHandler& handler)
{
    auto subscriber = std::make_shared<CommandSubscriber>(handler);
    CommandBus::subscribe(ev_type, subscriber);
    return subscriber;
}

void Enigma::Frameworks::releaseSubscription(const std::type_info& ev_type, const CommandSubscriberPtr& subscriber)
{
    CommandBus::unsubscribe(ev_type, subscriber);
}
