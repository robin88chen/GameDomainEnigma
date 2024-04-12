/*********************************************************************
 * \file   ViewerEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _VIEWER_EVENTS_H
#define _VIEWER_EVENTS_H

#include "Frameworks/Event.h"
#include "SceneGraph/SpatialId.h"
#include "Primitives/PrimitiveId.h"
#include <string>

namespace EnigmaViewer
{
    class AnimationClipItemUpdated : public Enigma::Frameworks::IEvent
    {
    public:
        AnimationClipItemUpdated(unsigned int cat_idx, unsigned int item_idx,
            const std::string& old_text, const std::string& new_text)
            : m_catIdx{ cat_idx }, m_itemIdx{ item_idx }, m_oldText{ old_text }, m_newText{ new_text } {}

        unsigned int categoryIndex() const { return m_catIdx; }
        unsigned int itemIndex() const { return m_itemIdx; }
        const std::string& oldText() const { return m_oldText; }
        const std::string& newText() const { return m_newText; }

    private:
        unsigned int m_catIdx;
        unsigned int m_itemIdx;
        std::string m_oldText;
        std::string m_newText;
    };
    class ViewingPawnConstituted : public Enigma::Frameworks::IEvent
    {
    public:
        ViewingPawnConstituted(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id) : m_id{ id }, m_primitiveId{ primitive_id } {}

        const Enigma::SceneGraph::SpatialId& id() const { return m_id; }
        const Enigma::Primitives::PrimitiveId& primitiveId() const { return m_primitiveId; }

    protected:
        Enigma::SceneGraph::SpatialId m_id;
        Enigma::Primitives::PrimitiveId m_primitiveId;
    };
    class ConstituteViewingPawnFailed : public Enigma::Frameworks::IEvent
    {
    public:
        ConstituteViewingPawnFailed(const Enigma::SceneGraph::SpatialId& id, std::error_code er) : m_id{ id }, m_error{ er } {}

        const Enigma::SceneGraph::SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        Enigma::SceneGraph::SpatialId m_id;
        std::error_code m_error;
    };
}
#endif // _VIEWER_EVENTS_H
