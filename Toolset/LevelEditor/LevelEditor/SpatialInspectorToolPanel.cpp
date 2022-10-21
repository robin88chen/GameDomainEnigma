#include "SpatialInspectorToolPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "propertygrid/pgitems.h"

using namespace LevelEditor;

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

enum class CategoryIndex
{
    Empty,
    Attributes,
    LocalSpatial,
    LocalBoundingBox,
    WorldSpatial,
    WorldBoundingBox
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

SpatialInspectorPanel::SpatialInspectorPanel(const nana::window& wd) : nana::panel<false>{ wd }
{
    m_place = nullptr;
    m_properties = nullptr;
}

SpatialInspectorPanel::~SpatialInspectorPanel()
{
    SAFE_DELETE(m_properties);
    SAFE_DELETE(m_place);
}

void SpatialInspectorPanel::Initialize(MainForm* form)
{
    m_mainForm = form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<property>");

    m_properties = new nana::propertygrid{ *this };
    UISchemeColors::ApplySchemaColors(m_properties->scheme());
    m_properties->ibox_show(false);
    m_place->field("property") << *m_properties;
    auto cat_attr = m_properties->append(TAG_ATTRIBUTES);
    cat_attr.append(std::make_unique<nana::pg_string>(TAG_SPATIAL_NAME, "name"));
    cat_attr.append(std::make_unique<nana::pg_string>(TAG_MODEL_NAME, "model"));
    cat_attr.append(std::make_unique<nana::pg_check>(TAG_VISIBLE, false));

    auto cat_spatial = m_properties->append(TAG_LOCAL_SPATIAL);
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_POSITION, "0, 0, 0"));
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_ROTATION, "0, 0, 0"));
    cat_spatial.append(std::make_unique<nana::pg_string>(TAG_SCALE, "1, 1, 1"));

    auto cat_local_bb = m_properties->append(TAG_LOCAL_BOUNDING);
    auto it = cat_local_bb.append(std::make_unique<nana::pg_string>(TAG_CENTER, "0, 0, 0"));
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

    m_properties->events().property_changed([this](auto arg) {this->OnPropertyChanged(arg); });
    m_place->collocate();
}

void SpatialInspectorPanel::Finalize()
{
    m_properties->clear();
}

void SpatialInspectorPanel::OnPropertyChanged(const nana::arg_propertygrid& arg)
{
}

void SpatialInspectorPanel::OnAttributePropertiesChanged(size_t index, const std::string& value)
{
}

void SpatialInspectorPanel::OnLocalSpatialPropertiesChanged(size_t index, const std::string& value)
{
}

void SpatialInspectorPanel::OnWorldSpatialPropertiesChanged(size_t index, const std::string& value)
{
}
