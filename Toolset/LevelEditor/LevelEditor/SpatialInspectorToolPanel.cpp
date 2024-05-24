#include "SpatialInspectorToolPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "propertygrid/pgitems.h"
#include "Frameworks/EventPublisher.h"
#include "LevelEditorEvents.h"
#include "Frameworks/StringFormat.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "EditorUtilities.h"
#include "SceneGraph/Light.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

#define TAG_ATTRIBUTES "Attributes"
#define TAG_SPATIAL_NAME "Spatial Name"
#define TAG_MODEL_NAME "Model Name"
#define TAG_VISIBLE "Visible"
#define TAG_LOCAL_SPATIAL "Local Spatial"
#define TAG_POSITION "Position"
#define TAG_ROTATION "Rotation"
#define TAG_SCALE "Scale"
#define TAG_WORLD_SPATIAL "World Spatial"
#define TAG_LOCAL_BOUNDING "Local Bounding Box"
#define TAG_WORLD_BOUNDING "World Bounding Box"
#define TAG_CENTER "Center"
#define TAG_AXIS_1 "Axis 1"
#define TAG_AXIS_2 "Axis 2"
#define TAG_AXIS_3 "Axis 3"
#define TAG_EXTENT "Extent"
#define TAG_LIGHT "Light"
#define TAG_COLOR "Color"
#define TAG_DIRECTION "Direction"
#define TAG_INTENSITY "Intensity"

enum class CategoryIndex
{
    Empty,
    Attributes,
    LocalSpatial,
    LocalBoundingBox,
    WorldSpatial,
    WorldBoundingBox,
};
enum class LightCategoryIndex
{
    Empty,
    Light,
};
enum class AttributePropertyIndex
{
    SpatialName,
    ModelName,
    Visibility,
};
enum class SpatialPropertyIndex
{
    Position,
    Rotation,
    Scale,
};
enum class BoundingBoxPropertyIndex
{
    Center,
    Axis1,
    Axis2,
    Axis3,
    Size,
};
enum class LightPropertyIndex
{
    Color,
    Direction,
    Position,
    Intensity,
};

SpatialInspectorPanel::SpatialInspectorPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_properties = nullptr;
    m_lightProperties = nullptr;
}

SpatialInspectorPanel::~SpatialInspectorPanel()
{
    SAFE_DELETE(m_properties);
    SAFE_DELETE(m_lightProperties);
    SAFE_DELETE(m_place);
}

void SpatialInspectorPanel::initialize(MainForm* form)
{
    m_mainForm = form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<property height=60%><light_property>");

    m_properties = new nana::propertygrid{ *this };
    UISchemeColors::applySchemaColors(m_properties->scheme());
    m_properties->ibox_show(false);
    m_lightProperties = new nana::propertygrid{ *this };
    UISchemeColors::applySchemaColors(m_lightProperties->scheme());
    m_lightProperties->ibox_show(false);
    m_place->field("property") << *m_properties;
    m_place->field("light_property") << *m_lightProperties;
    auto cat_attr = m_properties->append(TAG_ATTRIBUTES);
    auto it = cat_attr.append(std::make_unique<nana::pg_string>(TAG_SPATIAL_NAME, "name"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_attr.append(std::make_unique<nana::pg_string>(TAG_MODEL_NAME, "model"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    cat_attr.append(std::make_unique<nana::pg_check>(TAG_VISIBLE, false));

    auto cat_spatial = m_properties->append(TAG_LOCAL_SPATIAL);
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_POSITION, "0, 0, 0"));
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_ROTATION, "0, 0, 0"));
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_SCALE, "1, 1, 1"));

    auto cat_local_bb = m_properties->append(TAG_LOCAL_BOUNDING);
    it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_CENTER, "0, 0, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_1, "1, 0, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_2, "0, 1, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_3, "0, 0, 1"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_EXTENT, "1, 1, 1"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);

    auto cat_world = m_properties->append(TAG_WORLD_SPATIAL);
    cat_world.append(std::make_unique<nana::pg_string>(TAG_POSITION, "0, 0, 0"));

    auto cat_world_bb = m_properties->append(TAG_WORLD_BOUNDING);
    it = cat_world_bb.append(std::make_unique<nana::pg_string>(TAG_CENTER, "0, 0, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_world_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_1, "1, 0, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_world_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_2, "0, 1, 0"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_world_bb.append(std::make_unique<nana::pg_string>(TAG_AXIS_3, "0, 0, 1"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);
    it = cat_world_bb.append(std::make_unique<nana::pg_string>(TAG_EXTENT, "1, 1, 1"));
    dynamic_cast<nana::pg_string*>(it._m_pgitem())->editable(false);

    auto cat_light = m_lightProperties->append(TAG_LIGHT);
    cat_light.append(std::make_unique<nana::pg_string>(TAG_COLOR, "1, 1, 1, 1"));
    cat_light.append(std::make_unique<nana::pg_string>(TAG_DIRECTION, "1, 0, 0"));
    cat_light.append(std::make_unique<nana::pg_string>(TAG_POSITION, "0, 0, 0"));
    cat_light.append(std::make_unique<nana::pg_string>(TAG_INTENSITY, "1"));

    m_properties->events().property_changed([this](auto arg) {this->onPropertyChanged(arg); });
    m_lightProperties->events().property_changed([this](auto arg) {this->onLightPropertyChanged(arg); });
    m_place->collocate();
}

void SpatialInspectorPanel::finalize()
{
    m_properties->clear();
}

void SpatialInspectorPanel::subscribeHandlers()
{
    m_onPickedSpatialChanged = std::make_shared<EventSubscriber>([=](auto e) { onPickedSpatialChanged(e); });
    EventPublisher::subscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
}

void SpatialInspectorPanel::unsubscribeHandlers()
{
    EventPublisher::unsubscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
    m_onPickedSpatialChanged = nullptr;
}

void SpatialInspectorPanel::onPropertyChanged(const nana::arg_propertygrid& arg)
{
    auto pos = arg.item.pos();
    switch (pos.cat)
    {
    case static_cast<size_t>(CategoryIndex::Attributes):
        onAttributePropertiesChanged(pos.item, arg.item.value());
        break;
    case static_cast<size_t>(CategoryIndex::LocalSpatial):
        onLocalSpatialPropertiesChanged(pos.item, arg.item.value());
        break;
    case static_cast<size_t>(CategoryIndex::WorldSpatial):
        onWorldSpatialPropertiesChanged(pos.item, arg.item.value());
        break;
    default:
        break;
    }
}

void SpatialInspectorPanel::onLightPropertyChanged(const nana::arg_propertygrid& arg)
{
    if (m_selectedSpatial.expired()) return;
    auto light = std::dynamic_pointer_cast<Enigma::SceneGraph::Light>(m_selectedSpatial.lock());
    if (!light) return;
    auto pos = arg.item.pos();
    if (pos.cat != static_cast<size_t>(LightCategoryIndex::Light)) return;
    size_t index = pos.item;
    const std::string& value = arg.item.value();
    switch (index)
    {
    case static_cast<size_t>(LightPropertyIndex::Color):
        if (auto [color, isParseOk] = parseTextToColorRGBA(value); isParseOk)
        {
            light->setLightColor(color);
        }
        break;
    case static_cast<size_t>(LightPropertyIndex::Direction):
        if (auto [dir, isParseOk] = parseTextToVector3(value); isParseOk)
        {
            light->setLightDirection(dir);
        }
        break;
    case static_cast<size_t>(LightPropertyIndex::Intensity):
        float range = std::strtof(value.c_str(), nullptr);
        if (range != HUGE_VALF) light->setLightRange(range);
        break;
    }
}

void SpatialInspectorPanel::onAttributePropertiesChanged(size_t index, const std::string& value)
{
    if (m_selectedSpatial.expired()) return;
    switch (index)
    {
    case static_cast<size_t>(AttributePropertyIndex::ModelName):
        break;
    case static_cast<size_t>(AttributePropertyIndex::Visibility):
        if (value == "true")
        {
            m_selectedSpatial.lock()->removeSpatialFlag(Enigma::SceneGraph::Spatial::Spatial_Hide);
        }
        else
        {
            m_selectedSpatial.lock()->addSpatialFlag(Enigma::SceneGraph::Spatial::Spatial_Hide);
        }
        break;
    }
}

void SpatialInspectorPanel::onLocalSpatialPropertiesChanged(size_t index, const std::string& value)
{
    if (m_selectedSpatial.expired()) return;
    switch (index)
    {
    case static_cast<size_t>(SpatialPropertyIndex::Position):
        if (auto [pos, isParseOk] = parseTextToVector3(value); isParseOk)
        {
            m_selectedSpatial.lock()->setLocalPosition(pos);
        }
        break;
    case static_cast<size_t>(SpatialPropertyIndex::Rotation):
        if (auto [rot, isParseOk] = parseTextToVector3(value); isParseOk)
        {
            m_selectedSpatial.lock()->setLocalEulerAngle(rot);
        }
        break;
    case static_cast<size_t>(SpatialPropertyIndex::Scale):
        if (auto [scale, isParseOk] = parseTextToVector3(value); isParseOk)
        {
            m_selectedSpatial.lock()->setLocalScale(scale);
        }
        break;
    default:
        break;
    }
    showSpatialProperties(m_selectedSpatial.lock());
}

void SpatialInspectorPanel::onWorldSpatialPropertiesChanged(size_t index, const std::string& value)
{
    if (m_selectedSpatial.expired()) return;
    switch (index)
    {
    case static_cast<size_t>(SpatialPropertyIndex::Position):
        if (auto [pos, isParseOk] = parseTextToVector3(value); isParseOk)
        {
            m_selectedSpatial.lock()->changeWorldPosition(pos, std::nullopt);
        }
        break;
    default:
        break;
    }
    showSpatialProperties(m_selectedSpatial.lock());
}

void SpatialInspectorPanel::showSpatialProperties(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial)
{
    if (!spatial) return;
    if (auto item = m_properties->find(TAG_ATTRIBUTES, TAG_SPATIAL_NAME); item != nullptr)
    {
        item.value(spatial->id().name());
    }
    if (auto item = m_properties->find(TAG_ATTRIBUTES, TAG_VISIBLE); item != nullptr)
    {
        bool is_hide = spatial->testSpatialFlag(Enigma::SceneGraph::Spatial::SpatialBit::Spatial_Hide);
        item.value(is_hide ? "F" : "T");
    }
    if (auto item = m_properties->find(TAG_LOCAL_SPATIAL, TAG_POSITION); item != nullptr)
    {
        Vector3 pos = spatial->getLocalPosition();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", pos.x(), pos.y(), pos.z());
        item.value(s);
    }
    if (auto item = m_properties->find(TAG_LOCAL_SPATIAL, TAG_ROTATION); item != nullptr)
    {
        Vector3 rot = spatial->getLocalEulerAngle();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", rot.x(), rot.y(), rot.z());
        item.value(s);
    }
    if (auto item = m_properties->find(TAG_LOCAL_SPATIAL, TAG_SCALE); item != nullptr)
    {
        Vector3 scale = spatial->getLocalScale();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", scale.x(), scale.y(), scale.z());
        item.value(s);
    }
    if (auto item = m_properties->find(TAG_WORLD_SPATIAL, TAG_POSITION); item != nullptr)
    {
        Vector3 pos = spatial->getWorldPosition();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", pos.x(), pos.y(), pos.z());
        item.value(s);
    }
    if (auto item = m_properties->find(TAG_LOCAL_BOUNDING, TAG_CENTER); item != nullptr)
    {
        Vector3 pos = spatial->getModelBound().Center();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", pos.x(), pos.y(), pos.z());
        item.value(s);
    }
    if (auto item = m_properties->find(TAG_WORLD_BOUNDING, TAG_CENTER); item != nullptr)
    {
        Vector3 pos = spatial->getWorldBound().Center();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", pos.x(), pos.y(), pos.z());
        item.value(s);
    }
    auto box = spatial->getModelBound().BoundingBox3();
    if (box)
    {
        if (auto item = m_properties->find(TAG_LOCAL_BOUNDING, TAG_AXIS_1); item != nullptr)
        {
            Vector3 axis = box->Axis(0);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_LOCAL_BOUNDING, TAG_AXIS_2); item != nullptr)
        {
            Vector3 axis = box->Axis(1);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_LOCAL_BOUNDING, TAG_AXIS_3); item != nullptr)
        {
            Vector3 axis = box->Axis(2);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_LOCAL_BOUNDING, TAG_EXTENT); item != nullptr)
        {
            float* extent = box->Extent();
            std::string s = string_format("%6.2f, %6.2f, %6.2f", extent[0], extent[1], extent[2]);
            item.value(s);
        }
    }
    box = spatial->getWorldBound().BoundingBox3();
    if (box)
    {
        if (auto item = m_properties->find(TAG_WORLD_BOUNDING, TAG_AXIS_1); item != nullptr)
        {
            Vector3 axis = box->Axis(0);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_WORLD_BOUNDING, TAG_AXIS_2); item != nullptr)
        {
            Vector3 axis = box->Axis(1);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_WORLD_BOUNDING, TAG_AXIS_3); item != nullptr)
        {
            Vector3 axis = box->Axis(2);
            std::string s = string_format("%6.2f, %6.2f, %6.2f", axis.x(), axis.y(), axis.z());
            item.value(s);
        }
        if (auto item = m_properties->find(TAG_WORLD_BOUNDING, TAG_EXTENT); item != nullptr)
        {
            float* extent = box->Extent();
            std::string s = string_format("%6.2f, %6.2f, %6.2f", extent[0], extent[1], extent[2]);
            item.value(s);
        }
    }

    showLightProperties(std::dynamic_pointer_cast<Enigma::SceneGraph::Light>(spatial));
}

void SpatialInspectorPanel::showLightProperties(const std::shared_ptr<Enigma::SceneGraph::Light>& light)
{
    if (!light)
    {
        m_lightProperties->enabled(false);
        return;
    }
    m_lightProperties->enabled(true);
    Enigma::SceneGraph::LightInfo::LightType lit_type = light->info().lightType();
    if (auto item = m_lightProperties->find(TAG_LIGHT, TAG_COLOR); item != nullptr)
    {
        ColorRGBA color = light->getLightColor();
        std::string s = string_format("%5.2f, %5.2f, %5.2f, %5.2f", color.R(), color.G(), color.B(), color.A());
        item.value(s);
    }
    if (auto item = m_lightProperties->find(TAG_LIGHT, TAG_DIRECTION); item != nullptr)
    {
        Vector3 dir = light->getLightDirection();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", dir.x(), dir.y(), dir.z());
        item.value(s);
        item.enabled(lit_type == Enigma::SceneGraph::LightInfo::LightType::SunLight || lit_type == Enigma::SceneGraph::LightInfo::LightType::Directional);
    }
    if (auto item = m_lightProperties->find(TAG_LIGHT, TAG_POSITION); item != nullptr)
    {
        Vector3 pos = light->getLightPosition();
        std::string s = string_format("%6.2f, %6.2f, %6.2f", pos.x(), pos.y(), pos.z());
        item.value(s);
        item.enabled(lit_type == Enigma::SceneGraph::LightInfo::LightType::Point);
    }
    if (auto item = m_lightProperties->find(TAG_LIGHT, TAG_INTENSITY); item != nullptr)
    {
        float range = light->getLightRange();
        std::string s = string_format("%8.3f", range);
        item.value(s);
        item.enabled(lit_type == Enigma::SceneGraph::LightInfo::LightType::Point);
    }
}


void SpatialInspectorPanel::onPickedSpatialChanged(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PickedSpatialChanged>(e);
    if (!ev) return;

    m_selectedSpatial = std::make_shared<Enigma::SceneGraph::QueryRunningSpatial>(ev->id())->dispatch();
    if (!m_selectedSpatial.expired()) showSpatialProperties(m_selectedSpatial.lock());
}
