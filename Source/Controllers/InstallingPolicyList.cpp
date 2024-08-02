﻿#include "InstallingPolicyList.h"
#include "GameEngine/EngineInstallingPolicy.h"
#include "GameEngine/DeviceCreatingPolicy.h"
#include "Animators/AnimatorInstallingPolicy.h"
#include "Renderer/RendererInstallingPolicy.h"
#include "SceneGraph/SceneGraphInstallingPolicy.h"
#include "InputHandlers/InputHandlerInstallingPolicy.h"
#include "GameCommon/GameCommonInstallingPolicies.h"
#include "Rendering/SceneRendererInstallingPolicy.h"
#include "Primitives/PrimitiveRepositoryInstallingPolicy.h"
#include "GameEngine/EffectMaterialSourceRepositoryInstallingPolicy.h"
#include "GameEngine/TextureRepositoryInstallingPolicy.h"
#include "Geometries/GeometryInstallingPolicy.h"
#include "ShadowMap/ShadowMapInstallingPolicies.h"
#include "Terrain/TerrainInstallingPolicy.h"
#include "WorldMap/WorldMapInstallingPolicy.h"
#include "Renderables/RenderablesInstallingPolicy.h"

using namespace Enigma::Controllers;

constexpr std::uint16_t OrderOfDeviceCreating = 0;
constexpr std::uint16_t OrderOfEngineInstalling = 10;
constexpr std::uint16_t OrderOfGeometryInstalling = 15;
constexpr std::uint16_t OrderOfAnimatorInstalling = 20;
constexpr std::uint16_t OrderOfEffectMaterialSourceRepositoryInstalling = 25;
constexpr std::uint16_t OrderOfTextureInstalling = 26;
constexpr std::uint16_t OrderOfPrimitiveInstalling = 29;
constexpr std::uint16_t OrderOfRenderSystemInstalling = 30;
constexpr std::uint16_t OrderOfRenderableInstalling = 35;
constexpr std::uint16_t OrderOfSceneGraphInstalling = 40;
constexpr std::uint16_t OrderOfTerrainInstalling = 45;
constexpr std::uint16_t OrderOfWorldMapInstalling = 46;
constexpr std::uint16_t OrderOfDefaultRendererInstalling = 50;
constexpr std::uint16_t OrderOfInputHandlerInstalling = 60;

constexpr std::uint16_t OrderOfGameCameraInstalling = 500;
constexpr std::uint16_t OrderOfGameSceneInstalling = 510;

constexpr std::uint16_t OrderOfSceneRendererInstalling = 600;
constexpr std::uint16_t OrderOfShadowMapInstalling = 610;

constexpr std::uint16_t OrderOfGamePawnsInstalling = 700;

constexpr std::uint64_t OrderOfUnknownPolicy = UINT16_MAX;

InstallingPolicyList::InstallingPolicyList(std::initializer_list<std::shared_ptr<Engine::InstallingPolicy>> initializer_list)
    : std::list<std::shared_ptr<Engine::InstallingPolicy>>(initializer_list)
{
    sortOrder();
}

void InstallingPolicyList::sortOrder()
{
    makeOrderMap();
    sort([this](const std::shared_ptr<Engine::InstallingPolicy>& lhs, const std::shared_ptr<Engine::InstallingPolicy>& rhs)
        {
            auto& l = *lhs;
            auto& r = *rhs;
            return m_orderMap[typeid(l)] < m_orderMap[typeid(r)];
        });
}

void InstallingPolicyList::makeOrderMap()
{
    if (!m_orderMap.empty()) return;

    m_orderMap[typeid(Engine::DeviceCreatingPolicy)] = OrderOfDeviceCreating;
    m_orderMap[typeid(Engine::EngineInstallingPolicy)] = OrderOfEngineInstalling;
    m_orderMap[typeid(Geometries::GeometryInstallingPolicy)] = OrderOfGeometryInstalling;
    m_orderMap[typeid(Animators::AnimatorInstallingPolicy)] = OrderOfAnimatorInstalling;
    m_orderMap[typeid(Primitives::PrimitiveRepositoryInstallingPolicy)] = OrderOfPrimitiveInstalling;
    m_orderMap[typeid(Engine::EffectMaterialSourceRepositoryInstallingPolicy)] = OrderOfEffectMaterialSourceRepositoryInstalling;
    m_orderMap[typeid(Engine::TextureRepositoryInstallingPolicy)] = OrderOfTextureInstalling;
    m_orderMap[typeid(Renderer::RenderSystemInstallingPolicy)] = OrderOfRenderSystemInstalling;
    m_orderMap[typeid(Renderables::RenderablesInstallingPolicy)] = OrderOfRenderableInstalling;
    m_orderMap[typeid(Renderer::DefaultRendererInstallingPolicy)] = OrderOfDefaultRendererInstalling;
    m_orderMap[typeid(SceneGraph::SceneGraphInstallingPolicy)] = OrderOfSceneGraphInstalling;
    m_orderMap[typeid(Terrain::TerrainInstallingPolicy)] = OrderOfTerrainInstalling;
    m_orderMap[typeid(WorldMap::WorldMapInstallingPolicy)] = OrderOfWorldMapInstalling;
    m_orderMap[typeid(InputHandlers::InputHandlerInstallingPolicy)] = OrderOfInputHandlerInstalling;
    m_orderMap[typeid(GameCommon::GameCameraInstallingPolicy)] = OrderOfGameCameraInstalling;
    m_orderMap[typeid(GameCommon::GameSceneInstallingPolicy)] = OrderOfGameSceneInstalling;
    m_orderMap[typeid(GameCommon::SceneRendererInstallingPolicy)] = OrderOfSceneRendererInstalling;
    m_orderMap[typeid(GameCommon::DeferredRendererInstallingPolicy)] = OrderOfSceneRendererInstalling;
    m_orderMap[typeid(ShadowMap::ShadowMapInstallingPolicy)] = OrderOfShadowMapInstalling;
    m_orderMap[typeid(ShadowMap::CascadeShadowMapInstallingPolicy)] = OrderOfShadowMapInstalling;
    m_orderMap[typeid(GameCommon::AnimatedPawnInstallingPolicy)] = OrderOfGamePawnsInstalling;
}
