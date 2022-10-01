#include "SceneGraphRepository.h"
#include "Camera.h"
#include "Frustum.h"
#include <cassert>

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, SceneGraphRepository);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, SceneGraph, SceneGraphRepository, ISystemService);
    m_handSystem = GraphicCoordSys::LeftHand;
    m_needTick = false;
}

SceneGraphRepository::~SceneGraphRepository()
{
}

void SceneGraphRepository::SetCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::GetCoordinateSystem()
{
    return m_handSystem;
}

std::shared_ptr<Camera> SceneGraphRepository::CreateCamera(const std::string& name)
{
    assert(!HasCamera(name));
    auto camera = std::make_shared<Camera>(name, m_handSystem);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert({ name, camera });
    return camera;
}

bool SceneGraphRepository::HasCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    return ((it != m_cameras.end()) && (!it->second.expired()));
}

std::shared_ptr<Camera> SceneGraphRepository::QueryCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    if (it == m_cameras.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Frustum> SceneGraphRepository::CreateFrustum(const std::string& name, Frustum::ProjectionType proj)
{
    assert(!HasFrustum(name));
    auto frustum = std::make_shared<Frustum>(name, m_handSystem, proj);
    std::lock_guard locker{ m_frustumMapLock };
    m_frustums.insert({ name, frustum });
    return frustum;
}

bool SceneGraphRepository::HasFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    return ((it != m_frustums.end()) && (!it->second.expired()));
}

std::shared_ptr<Frustum> SceneGraphRepository::QueryFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    if (it == m_frustums.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}
