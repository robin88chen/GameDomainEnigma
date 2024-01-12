/*********************************************************************
 * \file   TextureQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_QUERIES_H
#define TEXTURE_QUERIES_H

#include "Frameworks/Query.h"
#include "TextureId.h"

namespace Enigma::Engine
{
    class Texture;

    class QueryTexture : public Frameworks::Query<std::shared_ptr<Texture>>
    {
    public:
        QueryTexture(const TextureId& id) : m_id(id) {}

        const TextureId& id() const { return m_id; }

    protected:
        TextureId m_id;
    };
}

#endif // TEXTURE_QUERIES_H
