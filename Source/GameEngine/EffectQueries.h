/*********************************************************************
 * \file   EffectQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_QUERIES_H
#define EFFECT_QUERIES_H

#include "Frameworks/Query.h"
#include "EffectMaterialId.h"

namespace Enigma::Engine
{
    class EffectMaterial;

    class QueryEffectMaterial : public Frameworks::Query<std::shared_ptr<EffectMaterial>>
    {
    public:
        QueryEffectMaterial(const EffectMaterialId& id) : m_id(id) {}

        const EffectMaterialId& id() const { return m_id; }

    protected:
        EffectMaterialId m_id;
    };
}

#endif // EFFECT_QUERIES_H
