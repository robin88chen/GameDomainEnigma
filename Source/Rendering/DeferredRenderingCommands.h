/*********************************************************************
 * \file   DeferredRenderingCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERING_COMMANDS_H
#define DEFERRED_RENDERING_COMMANDS_H

#include "Frameworks/Command.h"
#include "SceneGraph/Pawn.h"

namespace Enigma::GameCommon
{
    class BindGBuffer : public Frameworks::ICommand
    {
    public:
        BindGBuffer(const std::shared_ptr<SceneGraph::Pawn>& pawn) : m_pawn(pawn) {}

        const std::shared_ptr<SceneGraph::Pawn>& GetPawn() const { return m_pawn; }

    protected:
        std::shared_ptr<SceneGraph::Pawn> m_pawn;
    };
}

#endif // DEFERRED_RENDERING_COMMANDS_H
