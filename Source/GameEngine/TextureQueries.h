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
    class RequestTextureConstitution : public Frameworks::Query<std::shared_ptr<Texture>>
    {
    public:
        RequestTextureConstitution(const TextureId& id, const GenericDto& dto) : m_id(id), m_dto(dto) {}
        const TextureId& id() { return m_id; }
        const GenericDto& dto() { return m_dto; }

    private:
        TextureId m_id;
        GenericDto m_dto;
    };
}

#endif // TEXTURE_QUERIES_H
