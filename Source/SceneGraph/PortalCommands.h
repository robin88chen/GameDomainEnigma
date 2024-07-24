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
    class OutRegionNode;

    class AttachManagementOutsideRegion : public Frameworks::ICommand
    {
    public:
        AttachManagementOutsideRegion(const std::shared_ptr<OutRegionNode>& outside_region) : m_node(outside_region) {}

        const std::shared_ptr<OutRegionNode>& getRegion() { return m_node; }

    protected:
        std::shared_ptr<OutRegionNode> m_node;
    };

}

#endif // PORTAL_COMMANDS_H
