#include "LightQuadPawn.h"
#include "LightingMeshQueries.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "LightingPawnAssemblers.h"
#include "SceneGraph/LightEvents.h"
#include "RenderingErrors.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::Rendering;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;
using namespace Enigma::Rendering;

DEFINE_RTTI(Rendering, LightQuadPawn, LightingPawn);

LightQuadPawn::LightQuadPawn(const SpatialId& id) : LightingPawn(id)
{
}

LightQuadPawn::~LightQuadPawn()
{
}

std::shared_ptr<LightQuadPawn> LightQuadPawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<LightQuadPawn>(id);
}

std::shared_ptr<SpatialAssembler> LightQuadPawn::assembler() const
{
    return std::make_shared<LightQuadPawnAssembler>(m_id);
}

void LightQuadPawn::assemble(const std::shared_ptr<SceneGraph::SpatialAssembler>& assembler)
{
    assert(assembler);
    LightingPawn::assemble(assembler);
    if (auto lightingPawnAssembler = std::dynamic_pointer_cast<LightQuadPawnAssembler>(assembler))
    {
        if (!m_hostLight.expired()) lightingPawnAssembler->hostLightId(m_hostLight.lock()->id());
    }
}

std::shared_ptr<SpatialDisassembler> LightQuadPawn::disassembler() const
{
    return std::make_shared<LightQuadPawnDisassembler>();
}

void LightQuadPawn::disassemble(const std::shared_ptr<SceneGraph::SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    LightingPawn::disassemble(disassembler);
    if (auto lightingPawnDisassembler = std::dynamic_pointer_cast<LightQuadPawnDisassembler>(disassembler))
    {
        if (auto v = lightingPawnDisassembler->hostLightId())
        {
            m_hostLight = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(v.value())->dispatch());
        }
        if ((!m_primitive) && (!m_hostLight.expired()))
        {
            if ((m_hostLight.lock()->info().lightType() == LightInfo::LightType::Ambient) && lightingPawnDisassembler->primitiveId().has_value())
            {
                m_primitive = std::make_shared<RequestAmbientLightMeshAssembly>(lightingPawnDisassembler->primitiveId().value())->dispatch();
            }
            else if ((m_hostLight.lock()->info().lightType() == LightInfo::LightType::SunLight) && lightingPawnDisassembler->primitiveId().has_value())
            {
                m_primitive = std::make_shared<RequestSunLightMeshAssembly>(lightingPawnDisassembler->primitiveId().value())->dispatch();
            }
        }
    }
}

error LightQuadPawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    ColorRGBA colorLight(m_hostLight.lock()->info().color());
    RenderLightingState lighting_state;
    if (m_hostLight.lock()->info().lightType() == LightInfo::LightType::Ambient)
    {
        lighting_state.setAmbientLightColor(colorLight);
    }
    else if (m_hostLight.lock()->info().lightType() == LightInfo::LightType::SunLight)
    {
        Vector3 lightDir = m_hostLight.lock()->info().direction();
        lighting_state.setSunLight(lightDir, colorLight);
    }
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}


void LightQuadPawn::onLightInfoUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoUpdated>(e);
    if (!ev) return;
    if (getHostLight() == nullptr) return;
    if (ev->light() != getHostLight()) return;
    if ((ev->notifyCode() == LightInfoUpdated::NotifyCode::Color) || (ev->notifyCode() == LightInfoUpdated::NotifyCode::Direction))
    {
        notifySpatialRenderStateChanged();
    }
}
