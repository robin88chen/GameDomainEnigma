/*********************************************************************
 * \file   ViewerQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef VIEWER_QUERIES_H
#define VIEWER_QUERIES_H

#include "SceneGraph/SpatialId.h"
#include "Primitives/PrimitiveId.h"
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
    class RequestModelNames : public Enigma::Frameworks::Query<std::vector<std::string>>
    {
    public:
        RequestModelNames() = default;
    };
    class ResolveModelId : public Enigma::Frameworks::Query<std::optional<Enigma::Primitives::PrimitiveId>>
    {
    public:
        ResolveModelId(const std::string& model_name) : m_model_name(model_name) {}

        const std::string& modelName() const { return m_model_name; }

    protected:
        std::string m_model_name;
    };
    class HasAnimatedPawn : public Enigma::Frameworks::Query<bool>
    {
    public:
        HasAnimatedPawn(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

    protected:
        std::string m_name;
    };
    class RequestPawnNames : public Enigma::Frameworks::Query<std::vector<std::string>>
    {
    public:
        RequestPawnNames() = default;
    };
    class ResolvePawnId : public Enigma::Frameworks::Query<std::optional<Enigma::SceneGraph::SpatialId>>
    {
    public:
        ResolvePawnId(const std::string& pawn_name) : m_pawn_name(pawn_name) {}

        const std::string& pawnName() const { return m_pawn_name; }

    protected:
        std::string m_pawn_name;
    };
}

#endif // VIEWER_QUERIES_H
