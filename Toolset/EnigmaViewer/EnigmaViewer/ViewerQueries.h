/*********************************************************************
 * \file   ViewerQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef VIEWER_QUERIES_H
#define VIEWER_QUERIES_H

#include "Frameworks/Query.h"
#include "GameEngine/TextureId.h"

namespace EnigmaViewer
{
    class ResolveTextureId : public Enigma::Frameworks::Query<Enigma::Engine::TextureId>
    {
    public:
        ResolveTextureId(const std::string& filename) : m_filename(filename) {}

        const std::string& filename() const { return m_filename; }

    protected:
        std::string m_filename;
    };
}

#endif // VIEWER_QUERIES_H
