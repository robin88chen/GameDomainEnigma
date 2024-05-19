/*********************************************************************
 * \file   LevelEditorUiQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef LEVEL_EDITOR_UI_QUERIES_H
#define LEVEL_EDITOR_UI_QUERIES_H

#include "Frameworks/Query.h"
#include "GameEngine/TextureId.h"

namespace LevelEditor
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

#endif // LEVEL_EDITOR_UI_QUERIES_H
