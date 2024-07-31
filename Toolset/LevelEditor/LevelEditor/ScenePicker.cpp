﻿#include "ScenePicker.h"
#include "SceneGraph/Pawn.h"
#include "GameEngine/IntrBVRay3.h"
#include "Platforms/PlatformLayer.h"
#include "Primitives/IntrPrimitiveRay3.h"
#include "Terrain/TerrainPawn.h"
#include <algorithm>

using namespace LevelEditor;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;

ScenePicker::ScenePicker() : SceneTraveler()
{
    m_filter = FilterFlag::Pick_All;
}

ScenePicker::~ScenePicker()
{
}

SceneTraveler::TravelResult ScenePicker::travelTo(const SpatialPtr& spatial)
{
    if ((!spatial) || (spatial->getWorldBound().isEmpty())) return TravelResult::InterruptError;

    IntrBVRay3 intrBV(spatial->getWorldBound(), m_pickerRay);
    auto bv_test_res = intrBV.test(nullptr);
    if (!bv_test_res.m_hasIntersect) return TravelResult::TestFail;  // no intersection

    if ((m_filter & FilterFlag::Pick_Node) && (spatial->typeInfo().isDerived(Node::TYPE_RTTI)))
    {
        pushNodeRecord(spatial);
    }
    else if ((m_filter & FilterFlag::Pick_Pawn) && (Enigma::Frameworks::Rtti::isExactlyOrDerivedFrom(spatial->typeInfo().getName(), Enigma::Terrain::TerrainPawn::TYPE_RTTI.getName())))
    {
        bool test_result = pushPawnRecord(spatial);
        if (!test_result) return TravelResult::TestFail;
    }

    return TravelResult::Continue;
}

void ScenePicker::setAssociatedCamera(const std::shared_ptr<Camera>& camera)
{
    m_camera = camera;
}

void ScenePicker::setClippingCoord(const Vector2& clippingCoord)
{
    if (FATAL_LOG_EXPR(m_camera.expired())) return;

    Vector3 clip_vec = Vector3(clippingCoord.x(), clippingCoord.y(), 0.0f);
    Matrix4 mxProj = m_camera.lock()->projectionTransform().Inverse();
    Vector3 camera_vec = mxProj.TransformCoord(clip_vec);
    Matrix4 mxView = m_camera.lock()->viewTransform().Inverse();
    Vector3 world_vec = mxView.TransformCoord(camera_vec);

    Vector3 ray_dir = world_vec - m_camera.lock()->location();
    ray_dir.normalizeSelf();

    m_pickerRay = Ray3(m_camera.lock()->location(), ray_dir);
}

void ScenePicker::picking(const SpatialPtr& sceneRoot)
{
    m_pickRecords.clear();
    if (!sceneRoot) return;
    sceneRoot->visitBy(this);

    if (!m_pickRecords.empty())
    {
        sort(m_pickRecords.begin(), m_pickRecords.end(),
            [](const ScenePicker::PickerRecord& rec1, const ScenePicker::PickerRecord& rec2) -> bool
            { return (rec1.m_tParam < rec2.m_tParam); });
    }
}

unsigned int ScenePicker::getRecordCount()
{
    return static_cast<unsigned int>(m_pickRecords.size());
}

std::optional<ScenePicker::PickerRecord> ScenePicker::getPickRecord(unsigned int idx)
{
    if (idx >= m_pickRecords.size()) return std::nullopt;
    return m_pickRecords[idx];
}

void ScenePicker::pushNodeRecord(const SpatialPtr& spatial)
{
    NodePtr node = std::dynamic_pointer_cast<Node, Spatial>(spatial);
    if (!node) return;
    m_pickRecords.emplace_back(PickerRecord(node));
}

bool ScenePicker::pushPawnRecord(const SpatialPtr& spatial)
{
    auto pawn = std::dynamic_pointer_cast<Pawn>(spatial);
    if (!pawn) return false;
    if (!pawn->getPrimitive()) return false;

    IntrPrimitiveRay3 intrPrim(pawn->getPrimitive(), m_pickerRay);
    intrPrim.setRequiredResultCount(1);
    std::unique_ptr<IntersectorCache> cache = nullptr;
    if (m_intersectorCache.find(pawn->id()) != m_intersectorCache.end())
    {
        cache = std::move(m_intersectorCache.at(pawn->id()));
    }

    auto prim_find_res = intrPrim.find(std::move(cache));

    if (!prim_find_res.m_hasIntersect) return false;
    m_intersectorCache.insert_or_assign(pawn->id(), std::move(prim_find_res.m_cache));
    // copy result point
    if (intrPrim.getQuantity())
    {
        for (size_t i = 0; i < intrPrim.getQuantity(); i++)
        {
            float t = intrPrim.getRayT(i);
            m_pickRecords.emplace_back(PickerRecord(pawn, t, t * m_pickerRay.direction() + m_pickerRay.origin()));
        }
    }
    return true;
}
