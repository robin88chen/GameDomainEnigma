/*********************************************************************
 * \file   PortalCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef PORTAL_COMMANDS_H
#define PORTAL_COMMANDS_H

#include "Frameworks/Command.h"

namespace Enigma::SceneGraph
{
    class PortalZoneNode;

    class AttachPortalOutsideZone : public Frameworks::ICommand
    {
    public:
        AttachPortalOutsideZone(const std::shared_ptr<PortalZoneNode>& outside_node) : m_zone(outside_node) {}

        const std::shared_ptr<PortalZoneNode>& GetZone() { return m_zone; }

    protected:
        std::shared_ptr<PortalZoneNode> m_zone;
    };

}

#endif // PORTAL_COMMANDS_H
