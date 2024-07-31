#include "Event.h"
#include "EventPublisher.h"

using namespace Enigma::Frameworks;

void IEvent::publish()
{
    EventPublisher::publish(shared_from_this());
}

void IEvent::enqueue()
{
    EventPublisher::enqueue(shared_from_this());
}
