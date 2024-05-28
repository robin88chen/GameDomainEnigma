#include "Command.h"
#include "CommandBus.h"

using namespace Enigma::Frameworks;

void ICommand::execute()
{
    CommandBus::send(shared_from_this());
}

