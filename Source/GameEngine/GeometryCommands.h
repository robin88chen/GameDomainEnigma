/*********************************************************************
 * \file   GeometryCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_COMMANDS_H
#define _GEOMETRY_COMMANDS_H

#include "Frameworks/Command.h"
#include "GeometryDataPolicy.h"

namespace Enigma::Engine
{
    class BuildGeometryData : public Frameworks::ICommand
    {
    public:
        BuildGeometryData(const GeometryDataPolicy& policy) : m_policy(policy) {}
        const GeometryDataPolicy& GetPolicy() { return m_policy; }

    private:
        GeometryDataPolicy m_policy;
    };
}

#endif // _GEOMETRY_COMMANDS_H
