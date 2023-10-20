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
    class PrefabLoaded : public Frameworks::IResponseEvent
    {
    public:
        PrefabLoaded(const Frameworks::Ruid& request_ruid, const std::string& prefab_at_path) : IResponseEvent(request_ruid), m_prefabAtPath(prefab_at_path) {}

        const std::string& GetPrefabAtPath() const { return m_prefabAtPath; }

    private:
        std::string m_prefabAtPath;
    };
    class LoadPrefabFailed : public Frameworks::IResponseEvent
    {
    public:
        LoadPrefabFailed(const Frameworks::Ruid& request_ruid, std::error_code er) : IResponseEvent(request_ruid), m_error(er) {}

        std::error_code GetError() const { return m_error; }

    private:
        std::error_code m_error;
    };
}
#endif // PREFAB_EVENTS_H
