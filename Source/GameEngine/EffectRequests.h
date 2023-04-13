/*********************************************************************
 * \file   EffectRequests.h
 * \brief  effect requests
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _EFFECT_REQUESTS_H
#define _EFFECT_REQUESTS_H

#include "Frameworks/Request.h"
#include "EffectMaterialDto.h"

namespace Enigma::Engine
{
    class RequestCompileEffectMaterial : public Frameworks::IRequest
    {
    public:
        RequestCompileEffectMaterial(const EffectMaterialDto& dto) : m_dto(dto) {}

        const EffectMaterialDto& GetDto() const { return m_dto; }

    private:
        EffectMaterialDto m_dto;
    };
}

#endif // _EFFECT_REQUESTS_H
