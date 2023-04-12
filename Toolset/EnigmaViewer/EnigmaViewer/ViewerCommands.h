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
#include "Renderer/ModelPrimitive.h"
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
    class RefreshModelNodeTree : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshModelNodeTree(const std::shared_ptr<Enigma::Renderer::ModelPrimitive>& model) : m_model(model) {}

        const std::shared_ptr<Enigma::Renderer::ModelPrimitive>& GetModel() const { return m_model; }
    private:
        std::shared_ptr<Enigma::Renderer::ModelPrimitive> m_model;
    };;
}
#endif // _VIEWER_COMMANDS_H
