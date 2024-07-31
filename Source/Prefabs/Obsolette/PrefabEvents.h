/*********************************************************************
 * \file   PrefabEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PREFAB_EVENTS_H
#define PREFAB_EVENTS_H

#include "Frameworks/Event.h"
#include <system_error>

namespace Enigma::Prefabs
{
    class PawnPrefabLoaded : public Frameworks::IResponseEvent
    {
    public:
        PawnPrefabLoaded(const Frameworks::Ruid& request_ruid, const std::string& prefab_at_path, const std::shared_ptr<SceneGraph::Pawn>& pawn) : IResponseEvent(request_ruid), m_prefabAtPath(prefab_at_path), m_pawn(pawn) {}

        const std::string& getPrefabAtPath() const { return m_prefabAtPath; }
        const std::shared_ptr<SceneGraph::Pawn>& getPawn() const { return m_pawn; }

    private:
        std::string m_prefabAtPath;
        std::shared_ptr<SceneGraph::Pawn> m_pawn;
    };
    class LoadPawnPrefabFailed : public Frameworks::IResponseEvent
    {
    public:
        LoadPawnPrefabFailed(const Frameworks::Ruid& request_ruid, std::error_code er) : IResponseEvent(request_ruid), m_error(er) {}

        std::error_code GetError() const { return m_error; }

    private:
        std::error_code m_error;
    };
}
#endif // PREFAB_EVENTS_H
