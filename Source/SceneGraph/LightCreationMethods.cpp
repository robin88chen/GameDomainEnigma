#include "LightCreationMethods.h"
#include "Light.h"
#include "LightInfo.h"
#include "SpatialAssembler.h"

using namespace Enigma::SceneGraph;

std::shared_ptr<Light> LightCreationMethod::create(const SpatialId& id, const LightInfo& light_info)
{
    return std::make_shared<Light>(id, light_info);
}

std::shared_ptr<Light> LightCreationMethod::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    auto light = std::make_shared<Light>(id, LightInfo::LightType::Unknown);
    SpatialDisassembler::disassemble(light, dto);
    return light;
}

