/*********************************************************************
 * \file   ViewerCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _VIEWER_COMMANDS_H
#define _VIEWER_COMMANDS_H

#include "Frameworks/Command.h"
#include <string>

namespace EnigmaViewer
{
    class OutputMessage : public Enigma::Frameworks::ICommand
    {
    public:
        OutputMessage(const std::string& msg) : m_msg(msg) {}

        const std::string& GetMessage() const { return m_msg; }

    private:
        std::string m_msg;
    };
}
#endif // _VIEWER_COMMANDS_H
