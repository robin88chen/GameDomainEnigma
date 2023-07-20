/*********************************************************************
 * \file   DtoEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef DTO_EVENTS_H
#define DTO_EVENTS_H

#include "Frameworks/Event.h"
#include "Frameworks/ruid.h"
#include "GenericDto.h"

namespace Enigma::Engine
{
    class GenericDtoDeserialized : public Frameworks::IEvent
    {
    public:
        GenericDtoDeserialized(const Frameworks::Ruid& ruid, const GenericDtoCollection& dtos) :
            m_ruid(ruid), m_dtos(dtos) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        const GenericDtoCollection& GetDtos() { return m_dtos; }
    private:
        Frameworks::Ruid m_ruid;
        GenericDtoCollection m_dtos;
    };
    class DeserializeDtoFailed : public Frameworks::IEvent
    {
    public:
        DeserializeDtoFailed(const Frameworks::Ruid& ruid, std::error_code er) :
            m_ruid(ruid), m_error(er) {};
        const Frameworks::Ruid& GetRuid() { return m_ruid; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::error_code m_error;
    };
}

#endif // DTO_EVENTS_H
