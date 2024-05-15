/*********************************************************************
 * \file   ScenePIcker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef SCENE_PICKER_H
#define SCENE_PICKER_H

#include "SceneGraph/SceneTraveler.h"
#include "MathLib/Vector3.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Ray3.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Node.h"
#include "MathLib/IntersectorCache.h"
#include <vector>
#include <optional>

namespace LevelEditor
{
    class ScenePicker : public Enigma::SceneGraph::SceneTraveler
    {
    public:
        struct PickerRecord
        {
            std::weak_ptr<Enigma::SceneGraph::Spatial> m_spatial;
            float m_tParam;
            Enigma::MathLib::Vector3 m_vecIntersection;
            PickerRecord() : m_tParam(-Enigma::MathLib::Math::MAX_FLOAT), m_vecIntersection(Enigma::MathLib::Vector3::ZERO) {};
            PickerRecord(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial) : m_spatial(spatial), m_tParam(-Enigma::MathLib::Math::MAX_FLOAT), m_vecIntersection(Enigma::MathLib::Vector3::ZERO) {};
            PickerRecord(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial, float t, const Enigma::MathLib::Vector3& intr) :
                m_spatial(spatial), m_tParam(t), m_vecIntersection(intr) {};
        };

        enum FilterFlag
        {
            Pick_Node = 0x01,
            Pick_Pawn = 0x02,
            Pick_All = 0xff
        };
    public:
        ScenePicker();
        virtual ~ScenePicker() override;
        ScenePicker(const ScenePicker&) = default;
        ScenePicker(ScenePicker&&) = default;
        ScenePicker& operator=(const ScenePicker&) = default;
        ScenePicker& operator=(ScenePicker&&) = default;

        virtual TravelResult travelTo(const Enigma::SceneGraph::SpatialPtr& spatial) override;

        void setFilter(FilterFlag filter) { m_filter = filter; }
        FilterFlag getFilter() { return m_filter; }

        void setAssociatedCamera(const std::shared_ptr<Enigma::SceneGraph::Camera>& camera);
        void setClippingCoord(const Enigma::MathLib::Vector2& clippingCoord);

        void picking(const std::shared_ptr<Enigma::SceneGraph::Spatial>& sceneRoot);
        unsigned int getRecordCount();
        std::optional<PickerRecord> getPickRecord(unsigned int idx);

    private:
        void pushNodeRecord(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial);
        bool pushPawnRecord(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial);

    private:
        Enigma::MathLib::Ray3 m_pickerRay;
        std::weak_ptr<Enigma::SceneGraph::Camera> m_camera;
        std::vector<PickerRecord> m_pickRecords;
        FilterFlag m_filter;
        std::unordered_map<Enigma::SceneGraph::SpatialId, std::unique_ptr<Enigma::MathLib::IntersectorCache>, Enigma::SceneGraph::SpatialId::hash> m_intersectorCache;
    };
};


#endif // SCENE_PICKER_H
