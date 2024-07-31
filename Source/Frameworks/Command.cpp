﻿#include "Command.h"
#include "CommandBus.h"

using namespace Enigma::Frameworks;

void ICommand::execute()
{
    CommandBus::invoke(shared_from_this());
}

void ICommand::enqueue()
{
    CommandBus::enqueue(shared_from_this());
}
