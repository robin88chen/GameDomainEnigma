#include "LightCommands.h"
#include "SceneGraphQueries.h"
#include "SceneGraphCommands.h"
#include "Light.h"

using namespace Enigma::SceneGraph;

void ChangeLightColor::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setLightColor(color());
}

void ChangeLightDirection::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setLightDirection(direction());
}

void ChangeLightPos::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setLightPosition(position());
}

void ChangeLightAttenuation::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setLightAttenuation(MathLib::Vector3(constantFactor(), linearFactor(), quadraticFactor()));
}

void ChangeLightRange::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setLightRange(range());
}

void EnableLight::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setEnable(true);
}

void DisableLight::execute()
{
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(lightId())->dispatch());
    if (!light) return;
    light->setEnable(false);
}

void DeleteLight::execute()
{
    std::make_shared<DeleteSceneSpatial>(lightId())->execute();
}
