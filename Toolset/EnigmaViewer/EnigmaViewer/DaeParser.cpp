#include "DaeParser.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"
#include "Renderer/ModelPrimitive.h"
#include "GameEngine/GeometryDataDto.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "FileSystem/Filename.h"
#include "MathLib/ContainmentBox3.h"
#include "Gateways/DtoJsonGateway.h"
#include "GameEngine/TriangleList.h"
#include "SceneGraph/Pawn.h"
#include "Animators/AnimatorDtos.h"
#include "Animators/ModelAnimationAsset.h"
#include "Animators/AnimationAssetDtos.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include <sstream>

using namespace EnigmaViewer;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::SceneGraph;
using namespace Enigma::Animators;
using namespace Enigma::GameCommon;
using namespace Enigma::ShadowMap;

#define TOKEN_SCENE "scene"
#define TOKEN_INSTANCE_SCENE "instance_visual_scene"
#define TOKEN_SCENE_NODE "visual_scene"
#define TOKEN_NODE "node"
#define TOKEN_MATRIX    "matrix"
#define TOKEN_INSTANCE_GEOMETRY "instance_geometry"
#define TOKEN_GEOMETRY "geometry"
#define TOKEN_LIB_GEOMETRIES "library_geometries"
#define TOKEN_LIB_IMAGES "library_images"
#define TOKEN_LIB_MATERIALS "library_materials"
#define TOKEN_LIB_EFFECTS "library_effects"
#define TOKEN_LIB_VISUAL_SCENES "library_visual_scenes"
#define TOKEN_LIB_CONTROLLERS "library_controllers"
#define TOKEN_NAME "name"
#define TOKEN_MESH "mesh"
#define TOKEN_TRIANGLES "triangles"
#define TOKEN_COUNT "count"
#define TOKEN_INPUT "input"
#define TOKEN_SOURCE "source"
#define TOKEN_SEMANTIC "semantic"
#define TOKEN_VERTEX_SEMANTIC "VERTEX"
#define TOKEN_POSITION_SEMANTIC "POSITION"
#define TOKEN_NORMAL_SEMANTIC "NORMAL"
#define TOKEN_TEXCOORD_SEMANTIC "TEXCOORD"
#define TOKEN_VERTICES "vertices"
#define TOKEN_ID "id"
#define TOKEN_SET "set"
#define TOKEN_INDEX_ARRAY "p"
#define TOKEN_FLOAT_ARRAY "float_array"
#define TOKEN_OFFSET "offset"
#define TOKEN_INIT_FROM "init_from"
#define TOKEN_INSTANCE_EFFECT "instance_effect"
#define TOKEN_URL "url"
#define TOKEN_DIFFUSE "diffuse"
#define TOKEN_TEXTURE "texture"
#define TOKEN_TEXCOORD "texcoord"
#define TOKEN_MATERIAL "material"
#define TOKEN_EFFECT "effect"
#define TOKEN_IMAGE "image"
#define TOKEN_LIB_ANIMATIONS "library_animations"
#define TOKEN_ANIMATION "animation"
#define TOKEN_CHANNEL "channel"
#define TOKEN_TARGET "target"
#define TOKEN_SAMPLER "sampler"
#define TOKEN_INPUT_SEMANTIC "INPUT"
#define TOKEN_OUTPUT_SEMANTIC "OUTPUT"
#define TOKEN_CONTROLLER "controller"
#define TOKEN_SKIN "skin"
#define TOKEN_INSTANCE_CONTROLLER "instance_controller"
#define TOKEN_BIND_SHAPE_MATRIX "bind_shape_matrix"
#define TOKEN_VERTEX_WEIGHTS "vertex_weights"
#define TOKEN_JOINT_SEMANTIC "JOINT"
#define TOKEN_WEIGHT_SEMANTIC "WEIGHT"
#define TOKEN_VERTEX_WEIGHT_COUNT "vcount"
#define TOKEN_JOINT_WEIGHT_PAIR "v"
#define TOKEN_NAME_ARRAY "Name_array"
#define TOKEN_SURFACE "surface"
#define SUFFIX_IMAGE "-image"
#define SUFFIX_SAMPLER "-sampler"
#define SUFFIX_ANIM_TRANSFORM "/transform"

#define DIFFUSE_MAP_SEMANTIC "DiffuseMap"

#define MAX_WEIGHT_COUNT 4

DaeParser::DaeParser(const std::shared_ptr<GeometryRepository>& geometry_repository)
{
    m_geometryRepository = geometry_repository;
    m_config.LoadConfig();
}

DaeParser::~DaeParser()
{

}

void DaeParser::LoadDaeFile(const std::string& filename)
{
    IFilePtr file = FileSystem::Instance()->OpenFile(filename, Read | Binary, "");
    if (!file) return;
    unsigned file_size = static_cast<unsigned>(file->Size());
    auto read_buf = file->Read(0, file_size);
    FileSystem::Instance()->CloseFile(file);
    if (!read_buf)
    {
        OutputLog(filename + " read fail!!");
        return;
    }
    pugi::xml_document dae_doc;
    pugi::xml_parse_result res = dae_doc.load_buffer(&((read_buf.value())[0]), file_size);
    if (res.status != pugi::xml_parse_status::status_ok)
    {
        OutputLog(filename + " parse error!!");
        return;
    }
    else
    {
        OutputLog(filename + " parse done.");
    }
    m_filename = filename;
    pugi::xml_node collada_root = dae_doc.root().child("COLLADA");
    if (!collada_root)
    {
        OutputLog(filename + " not a COLLADA file!!");
    }
    m_pawn = AnimatedPawnDto();
    ParseScene(collada_root);
    ParseAnimations(collada_root);

    ComposeModelPrimitiveDto();
    m_pawn.isTopLevel() = true;
    /*if ((m_model) && (m_animation))
    {
        ModelPrimitiveAnimatorPtr model_anim = ModelPrimitiveAnimatorPtr{ menew ModelPrimitiveAnimator() };
        model_anim->LinkAnimationAsset(m_animation);
        m_model->SetAnimator(model_anim);
        for (unsigned int i = 0; i < m_model->GetMeshPrimitiveCount(); i++)
        {
            SkinMeshPrimitivePtr skin = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>
                (m_model->GetMeshPrimitive(i));
            if (skin)
            {
                model_anim->LinkSkinMesh(skin, m_skinBoneNames[skin->GetOwnerMeshNode()->GetName()]);
            }
        }
        Enigma::ServiceManager::GetSystemServiceAs<Enigma::AnimationFrameListener>()
            ->AddListeningAnimator(model_anim);
        model_anim->PlayAnimation(Enigma::AnimationClip(0.0f, 20.001f, Enigma::AnimationClip::WarpMode::Loop, 0));
    }
    if (m_model)
    {
        m_model->CalculateBoundingVolume(true);
    }
    mefree(file_buf);*/
}

void DaeParser::OutputLog(const std::string& msg)
{
    CommandBus::post(std::make_shared<OutputMessage>(msg));
}

void DaeParser::ComposeModelPrimitiveDto()
{
    ModelAnimatorDto animator_dto;
    animator_dto.AssetName() = m_animationAssetDto.getName();
    animator_dto.AssetFactoryDesc() = m_animationAssetDto.GetRtti();
    if ((animator_dto.AssetFactoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Native)
        || (animator_dto.AssetFactoryDesc().GetInstanceType() == FactoryDesc::InstanceType::ResourceAsset))
    {
        animator_dto.AnimationAssetDto() = m_animationAssetDto;
    }
    for (auto skin_bone_name : m_skinBoneNames)
    {
        SkinOperatorDto operator_dto;
        operator_dto.BoneNodeNames() = skin_bone_name.second;
        operator_dto.SkinMeshNodeName() = skin_bone_name.first;
        animator_dto.SkinOperators().emplace_back(operator_dto.toGenericDto());
    }
    m_model.TheAnimator() = animator_dto.toGenericDto();

    BoundingVolume unit_bv(Box3::UNIT_BOX);
    m_pawn.primitive() = m_model.toGenericDto();
    //m_pawn.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    m_pawn.name() = m_filename;
    m_pawn.localTransform() = Matrix4::IDENTITY;
    m_pawn.worldTransform() = Matrix4::IDENTITY;
    m_pawn.modelBound() = unit_bv.serializeDto().toGenericDto();
    m_pawn.worldBound() = unit_bv.serializeDto().toGenericDto();
    m_pawn.spatialFlag() = static_cast<unsigned>(SpatialShadowFlags::Spatial_ShadowCaster);
}

void DaeParser::ParseScene(const pugi::xml_node& collada_root)
{
    OutputLog("Start parse scene...");
    pugi::xml_node scene_node = collada_root.child(TOKEN_SCENE);
    if (!scene_node)
    {
        OutputLog(m_filename + " do not have any scene!!");
        return;
    }
    pugi::xml_node inst_scene = scene_node.child(TOKEN_INSTANCE_SCENE);
    if (!inst_scene)
    {
        OutputLog(m_filename + " do not have any instanced scene!!");
        return;
    }
    if (!inst_scene.attribute(TOKEN_URL))
    {
        OutputLog("instanced scene url error!!");
        return;
    }
    std::string scene_id = &(inst_scene.attribute(TOKEN_URL).as_string()[1]);
    pugi::xml_node model_scene_node = collada_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((strcmp(node.name(), TOKEN_SCENE_NODE) == 0) && (node.attribute(TOKEN_ID))
                && (scene_id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
    if (!model_scene_node)
    {
        OutputLog("can't find visual scene node " + scene_id + "!!");
        return;
    }
    ParseModelSceneNode(model_scene_node);
}

void DaeParser::ParseModelSceneNode(const pugi::xml_node& model_scene_node)
{
    OutputLog(" Parse model scene node "
        + std::string(model_scene_node.attribute(TOKEN_ID).as_string()) + "...");
    m_model = ModelPrimitiveDto();
    std::string model_name = model_scene_node.attribute(TOKEN_NAME).as_string();
    if (model_name == "Scene")
    {
        Filename filename(m_filename);
        model_name = filename.GetBaseFileName();
    }
    m_model.Name() = model_name;
    m_modelName = m_model.Name();
    MeshNodeTreeDto tree_dto;
    //m_model = std::make_shared<ModelPrimitive>(model_scene_node.attribute(TOKEN_NAME).as_string());
    pugi::xml_node scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNode(tree_dto, scene_node_xml, std::nullopt);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }
    m_model.TheNodeTree() = tree_dto.toGenericDto();

    // parse for skin
    /*scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNodeForSkin(scene_node_xml);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }*/

    //m_model->SelectVisualTechnique("Default");
}

void DaeParser::ParseSceneNode(MeshNodeTreeDto& node_tree, const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index)
{
    std::string node_id = std::string(scene_node_xml.attribute(TOKEN_ID).as_string());
    OutputLog("   Parse scene node " + node_id + "...");
    pugi::xml_node inst_controller = scene_node_xml.child(TOKEN_INSTANCE_CONTROLLER);
    if (inst_controller)
    {
        OutputLog("   " + node_id + " is a skin mesh node");
        ParseSceneNodeForSkin(node_tree, scene_node_xml, parent_node_array_index);
        return;  // this is a skin mesh node
    }

    std::string mesh_node_name = scene_node_xml.attribute(TOKEN_NAME).as_string();
    m_nodeIdNameMapping.emplace(node_id, mesh_node_name);
    std::string node_joint_name = scene_node_xml.attribute("sid").as_string();
    m_nodeJointIdMapping.emplace(node_joint_name, mesh_node_name);

    MeshNodeDto mesh_node;
    mesh_node.Name() = mesh_node_name;
    pugi::xml_node matrix_xml_node = scene_node_xml.child(TOKEN_MATRIX);
    if (matrix_xml_node)
    {
        Matrix4 mx;
        std::stringstream ss(std::string(matrix_xml_node.text().as_string()));
        for (int i = 0; i < 16; i++)
        {
            ss >> ((float*)mx)[i];
        }
        mesh_node.LocalT_PosTransform() = mx;
    }
    else
    {
        return; // this is not a mesh node
    }
    pugi::xml_node geometry_inst = scene_node_xml.child(TOKEN_INSTANCE_GEOMETRY);
    if (geometry_inst)
    {
        ParseGeometryInstanceNode(mesh_node, geometry_inst);
    }
    if (parent_node_array_index) mesh_node.ParentIndexInArray() = parent_node_array_index.value();
    std::optional<unsigned> current_index = static_cast<unsigned>(node_tree.MeshNodes().size());
    node_tree.MeshNodes().emplace_back(mesh_node.toGenericDto());
    pugi::xml_node child_node = scene_node_xml.child(TOKEN_NODE);
    while (child_node)
    {
        ParseSceneNode(node_tree, child_node, current_index);
        child_node = child_node.next_sibling(TOKEN_NODE);
    }
}

void DaeParser::ParseSceneNodeForSkin(MeshNodeTreeDto& node_tree, const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index)
{
    std::string node_name = std::string(scene_node_xml.attribute(TOKEN_ID).as_string());
    OutputLog("   Parse skin mesh node " + node_name + "...");
    pugi::xml_node inst_controller = scene_node_xml.child(TOKEN_INSTANCE_CONTROLLER);
    if (!inst_controller)
    {
        OutputLog("   " + node_name + " is not a skin mesh node");
        return;
    }

    MeshNodeDto mesh_node;
    mesh_node.Name() = scene_node_xml.attribute(TOKEN_NAME).as_string();

    std::string controller_id = &(inst_controller.attribute(TOKEN_URL).as_string()[1]);
    pugi::xml_node controller_node = FindNodeWithId(scene_node_xml.root(), TOKEN_CONTROLLER, controller_id);
    if (!controller_node)
    {
        OutputLog("can't find controller " + controller_id);
        return;
    }
    pugi::xml_node skin_node = controller_node.child(TOKEN_SKIN);
    if (!skin_node)
    {
        OutputLog("no skin define!!");
        return;
    }
    ClearParsedDatas();
    ParseSkinMesh(mesh_node, skin_node);
    if (parent_node_array_index) mesh_node.ParentIndexInArray() = parent_node_array_index.value();
    node_tree.MeshNodes().emplace_back(mesh_node.toGenericDto());
    /*mesh_node->SetParentIndexInArray(parent_node_array_index);
    mesh_node->SetDataStatus(Object::DataStatus::Ready);
    if (m_model)
    {
        m_model->GetMeshNodeTree()->AddMeshNode(mesh_node);
    }*/
}

void DaeParser::ParseGeometryInstanceNode(MeshNodeDto& mesh_node, const pugi::xml_node& geometry_inst)
{
    std::string geometry_url = &(geometry_inst.attribute(TOKEN_URL).as_string()[1]);
    pugi::xml_node geometry_node = geometry_inst.root().find_node([=](pugi::xml_node node) -> bool
        {
            if ((strcmp(node.name(), TOKEN_GEOMETRY) == 0) && (node.attribute(TOKEN_ID))
                && (geometry_url == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
    if (!geometry_node)
    {
        OutputLog("can't find geometry " + geometry_url + "!!");
        return;
    }
    ClearParsedDatas();
    ParseSingleGeometry(mesh_node, geometry_node, false);
}

void DaeParser::ParseSingleGeometry(MeshNodeDto& mesh_node, const pugi::xml_node& geometry_node, bool is_skin)
{
    pugi::xml_attribute attr_name = geometry_node.attribute(TOKEN_NAME);
    if (!attr_name)
    {
        OutputLog("geometry do not have name!!");
    }
    std::string geo_name = m_modelName + "." + attr_name.as_string();
    OutputLog("  Parse Geometry " + std::string(attr_name.as_string()));
    GeometryValueOffsets offsets;
    std::string material_id = "";
    std::tie(material_id, offsets) = ParseGeometryMesh(geometry_node.child(TOKEN_MESH));
    bool is_split_vertex = true;
    if (offsets.nor_offset >= 0)
    {
        is_split_vertex = false;
    }
    for (int i = 0; i < 4; i++)
    {
        if (offsets.tex_offset[i] >= 0) is_split_vertex = false;
    }
    std::string texmap_filename = "";
    if (material_id.length() > 0)
    {
        texmap_filename = FindMaterialTexImageFilename(geometry_node.root(), material_id);
    }
    SplitVertexPositions(offsets.pos_offset, 0, offsets.tex_offset[0], is_split_vertex, is_skin);
    CollapseVertexNormals(offsets.pos_offset, offsets.nor_offset, is_split_vertex);
    CollapseIndexArray(offsets.pos_offset, is_split_vertex);

    auto geo_dto = ComposeTriangleListDto(geo_name, is_skin);
    if (is_skin)
    {
        SkinMeshPrimitiveDto dto;
        dto.Name() = attr_name.as_string();
        dto.GeometryName() = geo_name;
        dto.GeometryFactoryDesc() = geo_dto.GetRtti();
        //dto.TheGeometry() = geo_dto;
        if (texmap_filename.empty())
        {
            dto.Effects().emplace_back(MakeMaterialDto(m_config.DefaultColorMeshEffectName(), m_config.DefaultColorMeshEffectFilename()).toGenericDto());
        }
        else
        {
            dto.Effects().emplace_back(MakeMaterialDto(m_config.DefaultTexturedSkinMeshEffectName(), m_config.DefaultTexturedSkinMeshEffectFilename()).toGenericDto());
            dto.TextureMaps().emplace_back(MakeEffectTextureMapDto(texmap_filename, texmap_filename, DIFFUSE_MAP_SEMANTIC).toGenericDto());
        }
        mesh_node.TheMeshPrimitive() = dto.toGenericDto();
    }
    else
    {
        MeshPrimitiveDto dto;
        dto.Name() = attr_name.as_string();
        dto.GeometryName() = geo_name;
        dto.GeometryFactoryDesc() = geo_dto.GetRtti();
        //dto.TheGeometry() = geo_dto;
        if (texmap_filename.empty())
        {
            dto.Effects().emplace_back(MakeMaterialDto(m_config.DefaultColorMeshEffectName(), m_config.DefaultColorMeshEffectFilename()).toGenericDto());
        }
        else
        {
            dto.Effects().emplace_back(MakeMaterialDto(m_config.DefaultTexturedMeshEffectName(), m_config.DefaultTexturedMeshEffectFilename()).toGenericDto());
            dto.TextureMaps().emplace_back(MakeEffectTextureMapDto(texmap_filename, texmap_filename, DIFFUSE_MAP_SEMANTIC).toGenericDto());
        }
        mesh_node.TheMeshPrimitive() = dto.toGenericDto();
    }
}

void DaeParser::ParseSkinMesh(MeshNodeDto& mesh_node, const pugi::xml_node& skin_node_xml)
{
    pugi::xml_node matrix_xml_node = skin_node_xml.child(TOKEN_BIND_SHAPE_MATRIX);
    if (matrix_xml_node)
    {
        Matrix4 mx;
        std::stringstream ss(std::string(matrix_xml_node.text().as_string()));
        for (int i = 0; i < 16; i++)
        {
            ss >> ((float*)mx)[i];
        }
        mesh_node.LocalT_PosTransform() = mx;
    }

    pugi::xml_node vertex_weights_node_xml = skin_node_xml.child(TOKEN_VERTEX_WEIGHTS);
    if (!vertex_weights_node_xml)
    {
        OutputLog("no vertex weights define!!");
        return;
    }
    ParseVertexWeights(mesh_node, skin_node_xml, vertex_weights_node_xml);
    std::string geometry_url = &(skin_node_xml.attribute(TOKEN_SOURCE).as_string()[1]);
    pugi::xml_node geometry_node = FindNodeWithId(skin_node_xml.root(), TOKEN_GEOMETRY, geometry_url);
    if (!geometry_node)
    {
        OutputLog("can't find geometry " + geometry_url + "!!");
        return;
    }
    ParseSingleGeometry(mesh_node, geometry_node, true);

}

std::tuple<std::string, DaeParser::GeometryValueOffsets> DaeParser::ParseGeometryMesh(const pugi::xml_node& mesh_node)
{
    std::string material_id = "";
    GeometryValueOffsets offsets;
    memset(offsets.tex_offset, 0xff, sizeof(offsets.tex_offset));
    if (!mesh_node)
    {
        OutputLog("no mesh node!!");
        return { material_id, offsets };
    }
    pugi::xml_node triangles_node = mesh_node.child(TOKEN_TRIANGLES);
    if (!triangles_node)
    {
        OutputLog("no triangles define!!");
        return { material_id, offsets };
    }
    int triangle_count = triangles_node.attribute(TOKEN_COUNT).as_int();
    if (triangles_node.attribute(TOKEN_MATERIAL))
    {
        material_id = triangles_node.attribute(TOKEN_MATERIAL).as_string();
    }
    pugi::xml_node input_source = triangles_node.child(TOKEN_INPUT);
    while (input_source)
    {
        if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_VERTEX_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            offsets.pos_offset = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node vertices_node = FindNodeWithId(mesh_node, TOKEN_VERTICES, source_id);
            if (vertices_node)
            {
                pugi::xml_node vertices_input = vertices_node.child(TOKEN_INPUT);
                while (vertices_input)
                {
                    if (!vertices_input.attribute(TOKEN_SEMANTIC))
                    {
                        vertices_input = vertices_input.next_sibling(TOKEN_INPUT);
                        continue;
                    }
                    if (strcmp(vertices_input.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_POSITION_SEMANTIC) == 0)
                    {
                        std::string position_source_id = &(vertices_input.attribute(TOKEN_SOURCE).as_string()[1]);
                        pugi::xml_node position_source = FindNodeWithId(mesh_node, TOKEN_SOURCE, position_source_id);
                        if (position_source)
                        {
                            ParsePositionSource(position_source);
                        }
                    }
                    else if (strcmp(vertices_input.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_NORMAL_SEMANTIC) == 0)
                    {
                        std::string normal_source_id = &(vertices_input.attribute(TOKEN_SOURCE).as_string()[1]);
                        pugi::xml_node normal_source = FindNodeWithId(mesh_node, TOKEN_SOURCE, normal_source_id);
                        if (normal_source)
                        {
                            ParseNormalSource(normal_source);
                        }
                    }
                    else if (strcmp(vertices_input.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_TEXCOORD_SEMANTIC) == 0)
                    {
                        std::string texc_source_id = &(vertices_input.attribute(TOKEN_SOURCE).as_string()[1]);
                        pugi::xml_node texcoord_source = FindNodeWithId(mesh_node, TOKEN_SOURCE, texc_source_id);
                        if (texcoord_source)
                        {
                            ParseTexcoordSource(texcoord_source, 0);
                        }
                    }
                    vertices_input = vertices_input.next_sibling(TOKEN_INPUT);
                }
            }
            else
            {
                OutputLog("no vertices node!!");
            }
        }
        else if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_NORMAL_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            offsets.nor_offset = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node normal_source = FindNodeWithId(mesh_node, TOKEN_SOURCE, source_id);
            if (normal_source)
            {
                ParseNormalSource(normal_source);
            }
        }
        else if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_TEXCOORD_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            int texcoord_set = input_source.attribute(TOKEN_SET).as_int();
            offsets.tex_offset[texcoord_set] = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node texcoord_source = FindNodeWithId(mesh_node, TOKEN_SOURCE, source_id);
            if (texcoord_source)
            {
                ParseTexcoordSource(texcoord_source, texcoord_set);
            }
        }
        input_source = input_source.next_sibling();
    }
    pugi::xml_node index_ary_node = triangles_node.child(TOKEN_INDEX_ARRAY);
    if (index_ary_node)
    {
        ParseIndexArray(index_ary_node, triangle_count);
    }
    return { material_id, offsets };
}

void DaeParser::ParsePositionSource(const pugi::xml_node& position_source)
{
    OutputLog("    Parse position source " + std::string(position_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = position_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        OutputLog("no float array in position!!");
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_positions.resize(data_count / 3);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 3; i++)
    {
        // y,z 互換不做了
        //ss >> m_positions[i].X() >> m_positions[i].Z() >> m_positions[i].Y();
        ss >> m_positions[i].X() >> m_positions[i].Y() >> m_positions[i].Z();
    }
}

void DaeParser::ParseNormalSource(const pugi::xml_node& normal_source)
{
    OutputLog("    Parse normal source " + std::string(normal_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = normal_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        OutputLog("no float array in normal!!");
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_normals.resize(data_count / 3);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 3; i++)
    {
        // y,z 互換不做了
        //ss >> m_normals[i].X() >> m_normals[i].Z() >> m_normals[i].Y();
        ss >> m_normals[i].X() >> m_normals[i].Y() >> m_normals[i].Z();
    }
}

void DaeParser::ParseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set)
{
    OutputLog("    Parse tex coord source " + std::string(texcoord_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = texcoord_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        OutputLog("no float array in texcoord!!");
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_texcoords[texcoord_set].resize(data_count / 2);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 2; i++)
    {
        ss >> m_texcoords[texcoord_set][i].X() >> m_texcoords[texcoord_set][i].Y();
        m_texcoords[texcoord_set][i].Y() = 1.0f - m_texcoords[texcoord_set][i].Y();  // y 反向
    }
}

void DaeParser::ParseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count)
{
    OutputLog("    Parse index array ");
    m_primitiveIndices.resize(triangle_count * 3 * 3);
    std::stringstream ss(std::string(index_ary_node.text().as_string()));
    for (int i = 0; i < triangle_count * 3 * 3; i++)
    {
        ss >> m_primitiveIndices[i];
    }
}

void DaeParser::ParseVertexWeights(MeshNodeDto& mesh_node, const pugi::xml_node& skin_host_xml, const pugi::xml_node& vertex_weights_xml)
{
    OutputLog("  Parse Vertex Weights for " + mesh_node.Name());
    pugi::xml_node input_xml = vertex_weights_xml.child(TOKEN_INPUT);
    while (input_xml)
    {
        if (strcmp(input_xml.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_JOINT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_xml.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node source_xml = FindNodeWithId(skin_host_xml, TOKEN_SOURCE, source_id);
            if (source_xml)
            {
                ParseJointNameSource(mesh_node, source_xml);
            }
        }
        else if (strcmp(input_xml.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_WEIGHT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_xml.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node source_xml = FindNodeWithId(skin_host_xml, TOKEN_SOURCE, source_id);
            if (source_xml)
            {
                ParseWeightsArraySource(source_xml);
            }
        }
        input_xml = input_xml.next_sibling(TOKEN_INPUT);
    }
    pugi::xml_node weight_count_xml = vertex_weights_xml.child(TOKEN_VERTEX_WEIGHT_COUNT);
    if (!weight_count_xml)
    {
        OutputLog("no weight count define!!");
        return;
    }
    int data_count = vertex_weights_xml.attribute(TOKEN_COUNT).as_int();
    int max_weight_count = 0;
    m_weightCounts.resize(data_count);
    std::stringstream ss(std::string(weight_count_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> m_weightCounts[i];
        if (m_weightCounts[i] > max_weight_count) max_weight_count = m_weightCounts[i];
    }
    if (max_weight_count > MAX_WEIGHT_COUNT)
    {
        OutputLog("(not supported) max weight count > 4!!");
    }
    pugi::xml_node influence_pair_xml = vertex_weights_xml.child(TOKEN_JOINT_WEIGHT_PAIR);
    if (!influence_pair_xml)
    {
        OutputLog("no influence pair!!");
        return;
    }
    m_weightPaletteIndices.resize(data_count, 0xffffffff);
    m_vertexWeights.resize(data_count * MAX_WEIGHT_COUNT, 0.0f);
    ss = std::stringstream(std::string(influence_pair_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        unsigned char p[4] = { 0xff, 0xff, 0xff, 0xff };
        float w[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        for (int j = 0; j < m_weightCounts[i]; j++)
        {
            unsigned int b_idx;
            ss >> b_idx;
            unsigned int w_index;
            ss >> w_index;
            if (j >= MAX_WEIGHT_COUNT) continue;  // 超過max weight count, 不處理
            p[j] = (unsigned char)b_idx;
            //ss >> p[j];
            m_vertexWeights[i * MAX_WEIGHT_COUNT + j] = m_weightSource[w_index];
        }
        m_weightPaletteIndices[i] = (unsigned int)(p[3] << 24) + (unsigned int)(p[2] << 16)
            + (unsigned int)(p[1] << 8) + (unsigned int)(p[0]);
    }
}

void DaeParser::ParseJointNameSource(MeshNodeDto& mesh_node, const pugi::xml_node& bone_names_xml)
{
    pugi::xml_node name_array_xml = bone_names_xml.child(TOKEN_NAME_ARRAY);
    if (!name_array_xml)
    {
        OutputLog("no bone name array define!!");
        return;
    }
    int data_count = name_array_xml.attribute(TOKEN_COUNT).as_int();
    std::vector<std::string> bone_names;
    bone_names.resize(data_count, "");
    std::stringstream ss(std::string(name_array_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> bone_names[i];
    }
    m_skinBoneNames[mesh_node.Name()] = bone_names;
    for (std::string& joint : m_skinBoneNames[mesh_node.Name()])
    {
        joint = m_nodeJointIdMapping[joint];
    }
}

void DaeParser::ParseWeightsArraySource(const pugi::xml_node& weights_array_xml)
{
    pugi::xml_node data_array_xml = weights_array_xml.child(TOKEN_FLOAT_ARRAY);
    if (!data_array_xml)
    {
        OutputLog("no weight array source define!!");
        return;
    }
    int data_count = data_array_xml.attribute(TOKEN_COUNT).as_int();
    m_weightSource.resize(data_count, 0.0f);
    std::stringstream ss(std::string(data_array_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> m_weightSource[i];
    }
}

void DaeParser::ParseAnimations(const pugi::xml_node& collada_root)
{
    if (!collada_root) return;
    pugi::xml_node anim_lib_node = collada_root.child(TOKEN_LIB_ANIMATIONS);
    if (!anim_lib_node)
    {
        OutputLog("has no animations lib");
        return;
    }
    ModelAnimationAssetDto asset_dto = ModelAnimationAssetDto();
    asset_dto.Name() = m_modelName;
    pugi::xml_node anim_name_node = anim_lib_node.child(TOKEN_ANIMATION);
    if (anim_name_node)
    {
        pugi::xml_node anim_node = anim_name_node.child(TOKEN_ANIMATION);
        while (anim_node)
        {
            ParseSingleAnimation(asset_dto, anim_node);
            anim_node = anim_node.next_sibling(TOKEN_ANIMATION);
        }
        //anim_name_node = anim_name_node.next_sibling(TOKEN_ANIMATION);
    }
    //m_animation->SetDataStatus(Object::DataStatus::Ready);
    //m_animation->GetFactoryDesc().ClaimAsResourceAsset(m_filename, m_filename + ".anim");
    m_animationAssetDto = asset_dto.toGenericDto();
    FactoryDesc desc(ModelAnimationAsset::TYPE_RTTI.getName());
    desc.ClaimAsResourceAsset(m_modelName, "pawns/" + m_modelName + ".ani", "APK_PATH");
    m_animationAssetDto.AddRtti(desc);
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto>{m_animationAssetDto});
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("pawns/" + m_modelName + ".ani@APK_PATH"), Write | OpenAlways | Binary);
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    desc.ClaimFromResource(m_modelName, "pawns/" + m_modelName + ".ani", "APK_PATH");
    m_animationAssetDto.AddRtti(desc);
}

void DaeParser::ParseSingleAnimation(Enigma::Animators::ModelAnimationAssetDto& asset_dto, const pugi::xml_node& anim_node)
{
    if (!anim_node)
    {
        OutputLog("no anim node!!");
        return;
    }
    AnimationTimeSRTDto srt_data;
    pugi::xml_node channel_node = anim_node.child(TOKEN_CHANNEL);
    if (!channel_node)
    {
        OutputLog("animation has no channel!!");
        return;
    }
    std::string sampler_source_id;
    if (channel_node.attribute(TOKEN_SOURCE))
    {
        sampler_source_id = &(channel_node.attribute(TOKEN_SOURCE).as_string()[1]);
    }
    if (sampler_source_id.length() == 0)
    {
        OutputLog("anim source id empty!!");
        return;
    }
    pugi::xml_node sampler_node = FindNodeWithId(anim_node, TOKEN_SAMPLER, sampler_source_id);
    if (!sampler_node)
    {
        OutputLog("can't find anim sampler " + sampler_source_id + "!!");
        return;
    }
    ParseAnimationSample(srt_data, sampler_node, anim_node);

    std::string target_matrix_id;
    if (channel_node.attribute(TOKEN_TARGET))
    {
        target_matrix_id = channel_node.attribute(TOKEN_TARGET).as_string();
    }
    if (target_matrix_id.length() == 0)
    {
        OutputLog("anim target empty!!");
        return;
    }
    size_t pos = target_matrix_id.rfind(SUFFIX_ANIM_TRANSFORM);
    std::string target_mesh_node_id;
    if (pos != std::string::npos)
    {
        target_mesh_node_id = target_matrix_id.substr(0, pos);
    }
    if (target_mesh_node_id.length() == 0)
    {
        OutputLog("can't find target mesh node id from" + target_matrix_id + "!!");
        return;
    }
    std::string target_mesh_node_name = m_nodeIdNameMapping[target_mesh_node_id];
    if (target_mesh_node_name.length() == 0)
    {
        OutputLog("can't find animation target mesh node of " + target_mesh_node_id + "!!");
        return;
    }
    OutputLog("parse animation for node " + target_mesh_node_name + " done.");

    asset_dto.MeshNodeNames().emplace_back(target_mesh_node_name);
    asset_dto.TimeSRTs().emplace_back(srt_data.toGenericDto());
}

void DaeParser::ParseAnimationSample(AnimationTimeSRTDto& srt_data, const pugi::xml_node& sampler_node, const pugi::xml_node& anim_node)
{
    if (!sampler_node) return;
    m_timeValues.clear();
    m_animMatrixs.clear();
    pugi::xml_node input_node = sampler_node.child(TOKEN_INPUT);
    while (input_node)
    {
        if (!input_node.attribute(TOKEN_SEMANTIC))
        {
            input_node = input_node.next_sibling(TOKEN_INPUT);
            continue;
        }
        if (strcmp(input_node.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_INPUT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_node.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node time_value_source = FindNodeWithId(anim_node, TOKEN_SOURCE, source_id);
            if (time_value_source)
            {
                ParseTimeValue(time_value_source);
            }
        }
        else if (strcmp(input_node.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_OUTPUT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_node.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node anim_matrix_source = FindNodeWithId(anim_node, TOKEN_SOURCE, source_id);
            if (anim_matrix_source)
            {
                ParseAnimationMatrix(anim_matrix_source);
            }
        }
        input_node = input_node.next_sibling(TOKEN_INPUT);
    }
    AnalyzeSRTKeys(srt_data);
}

void DaeParser::ParseTimeValue(const pugi::xml_node& time_value_source)
{
    if (!time_value_source) return;
    unsigned int count = 0;
    pugi::xml_node data_ary = time_value_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary)
    {
        OutputLog("time value source has no data!!");
        return;
    }
    if (data_ary.attribute(TOKEN_COUNT))
    {
        count = data_ary.attribute(TOKEN_COUNT).as_uint();
    }
    if (count == 0)
    {
        OutputLog("time value count = 0 !!");
        return;
    }
    m_timeValues.resize(count);
    std::stringstream ss(std::string(data_ary.text().as_string()));
    for (unsigned int i = 0; i < count; i++)
    {
        ss >> m_timeValues[i];
    }
}

void DaeParser::ParseAnimationMatrix(const pugi::xml_node& anim_matrix_source)
{
    if (!anim_matrix_source) return;
    unsigned int count = 0;
    pugi::xml_node data_ary = anim_matrix_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary)
    {
        OutputLog("animation matrix source has no data!!");
        return;
    }
    if (data_ary.attribute(TOKEN_COUNT))
    {
        count = data_ary.attribute(TOKEN_COUNT).as_uint() / 16;
    }
    if (count == 0)
    {
        OutputLog("animation matrix count = 0 !!");
        return;
    }
    m_animMatrixs.resize(count);
    std::stringstream ss(std::string(data_ary.text().as_string()));
    for (unsigned int i = 0; i < count; i++)
    {
        for (unsigned int j = 0; j < 16; j++)
        {
            ss >> ((float*)m_animMatrixs[i])[j];
        }
    }
}

void DaeParser::AnalyzeSRTKeys(AnimationTimeSRTDto& srt_data)
{
    if (m_timeValues.size() == 0) return;
    AnimationTimeSRT::ScaleKey scale_key;
    AnimationTimeSRT::TranslateKey trans_key;
    AnimationTimeSRT::RotationKey rot_key;
    AnimationTimeSRT::ScaleKeyVector scale_keys;
    AnimationTimeSRT::TranslateKeyVector translate_keys;
    AnimationTimeSRT::RotationKeyVector rotation_keys;

    unsigned sample_count = static_cast<unsigned>(m_timeValues.size());
    if (m_animMatrixs.size() != sample_count)
    {
        OutputLog("number of time values & matrixs not match!!");
        return;
    }

    scale_keys.reserve(sample_count);
    translate_keys.reserve(sample_count);
    rotation_keys.reserve(sample_count);

    Quaternion last_qt;
    Vector3 last_trans = Vector3::ZERO;
    Vector3 last_scale = Vector3(1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < sample_count; i++)
    {
        float time_value = m_timeValues[i];
        std::tie(scale_key.m_vecKey, rot_key.m_qtKey, trans_key.m_vecKey) = m_animMatrixs[i].UnMatrixSRT();
        // fix invalid key values
        if ((!std::isfinite(scale_key.m_vecKey.X())) || (!std::isfinite(scale_key.m_vecKey.Y()))
            || (!std::isfinite(scale_key.m_vecKey.Z())))
        {
            scale_key.m_vecKey = last_scale; //Vector3::ZERO;
            rot_key.m_qtKey = last_qt; //Quaternion::ZERO;
        }
        if ((!std::isfinite(trans_key.m_vecKey.X())) || (!std::isfinite(trans_key.m_vecKey.Y()))
            || (!std::isfinite(trans_key.m_vecKey.Z())))
        {
            trans_key.m_vecKey = last_trans; //Vector3::ZERO;
        }
        if (i > 0)
        {
            if (last_qt.Dot(rot_key.m_qtKey) < 0)  // 這個東西是一個trick.... 我們如果確保兩個Quaternion之間夾角是最短路徑的話，算slerp時就不用強制取shortest path
            {
                rot_key.m_qtKey = -rot_key.m_qtKey;
            }
        }

        last_qt = rot_key.m_qtKey;
        last_scale = scale_key.m_vecKey;
        last_trans = trans_key.m_vecKey;

        if (i <= 1)  // 第一個key,第二個key先輸出
        {
            scale_key.m_time = time_value;
            scale_keys.push_back(scale_key);
            rot_key.m_time = time_value;
            rotation_keys.push_back(rot_key);
            trans_key.m_time = time_value;
            translate_keys.push_back(trans_key);
        }
        else
        {
            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((scale_key.m_vecKey == scale_keys[scale_keys.size() - 1].m_vecKey)
                && (scale_key.m_vecKey == scale_keys[scale_keys.size() - 2].m_vecKey))
            {
                scale_keys[scale_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                scale_key.m_time = time_value;
                scale_keys.push_back(scale_key);
            }

            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((rot_key.m_qtKey == rotation_keys[rotation_keys.size() - 1].m_qtKey)
                && (rot_key.m_qtKey == rotation_keys[rotation_keys.size() - 2].m_qtKey))
            {
                rotation_keys[rotation_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                rot_key.m_time = time_value;
                rotation_keys.push_back(rot_key);
            }

            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((trans_key.m_vecKey == translate_keys[translate_keys.size() - 1].m_vecKey)
                && (trans_key.m_vecKey == translate_keys[translate_keys.size() - 2].m_vecKey))
            {
                translate_keys[translate_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                trans_key.m_time = time_value;
                translate_keys.push_back(trans_key);
            }
        }
    }
    for (auto& key : scale_keys)
    {
        srt_data.ScaleTimeKeys().push_back(key.m_time);
        srt_data.ScaleTimeKeys().push_back(key.m_vecKey.X());
        srt_data.ScaleTimeKeys().push_back(key.m_vecKey.Y());
        srt_data.ScaleTimeKeys().push_back(key.m_vecKey.Z());
    }
    for (auto& key : rotation_keys)
    {
        srt_data.RotateTimeKeys().push_back(key.m_time);
        srt_data.RotateTimeKeys().push_back(key.m_qtKey.W());
        srt_data.RotateTimeKeys().push_back(key.m_qtKey.X());
        srt_data.RotateTimeKeys().push_back(key.m_qtKey.Y());
        srt_data.RotateTimeKeys().push_back(key.m_qtKey.Z());
    }
    for (auto& key : translate_keys)
    {
        srt_data.TranslateTimeKeys().push_back(key.m_time);
        srt_data.TranslateTimeKeys().push_back(key.m_vecKey.X());
        srt_data.TranslateTimeKeys().push_back(key.m_vecKey.Y());
        srt_data.TranslateTimeKeys().push_back(key.m_vecKey.Z());
    }
}

void DaeParser::SplitVertexPositions(int pos_offset, int tex_set, int tex_offset, bool is_split_vertex, bool is_skin)
{
    if (is_split_vertex)
    {
        m_splitedPositions = m_positions;
        return;
    }
    // 相同位置，但是不同的貼圖軸，要拆成不同的vertex
    m_splitedPositions.clear();
    m_splitedTexCoord[tex_set].clear();
    m_splitedWeightIndices.clear();
    m_splitedVertexWeights.clear();
    auto hash_fn = [](const std::pair<int, int> p) { return std::hash<int>()(p.first); };
    typedef std::unordered_map<std::pair<int, int>, int, decltype(hash_fn)> PosTexIndexMap;
    PosTexIndexMap mapPosTexIndex(10, hash_fn);
    int prim_set_count = static_cast<int>(m_primitiveIndices.size()) / 3;
    for (int i = 0; i < prim_set_count; i++)
    {
        int tex_idx = m_primitiveIndices[i * 3 + tex_offset];
        int pos_idx = m_primitiveIndices[i * 3 + pos_offset];
        PosTexIndexMap::iterator iter = mapPosTexIndex.find(std::make_pair(pos_idx, tex_idx));
        if (iter == mapPosTexIndex.end()) // a new combination
        {
            int new_pos_idx = static_cast<int>(m_splitedPositions.size());
            int new_tex_idx = static_cast<int>(m_splitedTexCoord[tex_set].size());
            m_splitedPositions.push_back(m_positions[pos_idx]);
            m_splitedTexCoord[tex_set].push_back(m_texcoords[tex_set][tex_idx]);
            if (is_skin)
            {
                m_splitedWeightIndices.push_back(m_weightPaletteIndices[pos_idx]);
                m_splitedVertexWeights.push_back(m_vertexWeights[pos_idx * MAX_WEIGHT_COUNT]);
                m_splitedVertexWeights.push_back(m_vertexWeights[pos_idx * MAX_WEIGHT_COUNT + 1]);
                m_splitedVertexWeights.push_back(m_vertexWeights[pos_idx * MAX_WEIGHT_COUNT + 2]);
                m_splitedVertexWeights.push_back(m_vertexWeights[pos_idx * MAX_WEIGHT_COUNT + 3]);
            }

            m_primitiveIndices[i * 3 + pos_offset] = new_pos_idx;
            m_primitiveIndices[i * 3 + tex_offset] = new_tex_idx;
            mapPosTexIndex.insert(std::make_pair(std::make_pair(pos_idx, tex_idx), new_pos_idx));
        }
        else
        {
            m_primitiveIndices[i * 3 + pos_offset] = iter->second;
            m_primitiveIndices[i * 3 + tex_offset] = iter->second;
        }
    }
}

void DaeParser::CollapseVertexNormals(int pos_offset, int nor_offset, bool is_split_vertex)
{
    if (is_split_vertex) // 不需要 collapse
    {
        m_collapsedNormals = m_normals;
        return;
    }
    if (nor_offset < 0) return;
    int vertex_count = static_cast<int>(m_splitedPositions.size());
    m_collapsedNormals.resize(vertex_count);
    int prim_set_count = static_cast<int>(m_primitiveIndices.size()) / 3;
    for (int i = 0; i < vertex_count; i++)
    {
        Vector3 nor = Vector3::ZERO;
        std::list<Vector3> used_normal;  // 要去除掉共平面的normal計算
        for (int p = 0; p < prim_set_count; p++)
        {
            if (m_primitiveIndices[p * 3 + pos_offset] == i) // 對應到頂點
            {
                Vector3 current_nor = m_normals[m_primitiveIndices[p * 3 + nor_offset]];
                if (std::find_if(used_normal.begin(), used_normal.end(),
                    [=](Vector3 vec) -> bool { return vec == current_nor; }) == used_normal.end())
                {
                    nor += current_nor;
                    used_normal.push_back(current_nor);
                }
            }
        }
        m_collapsedNormals[i] = nor.Normalize();
    }
}

void DaeParser::CollapseIndexArray(int pos_offset, bool is_split_vertex)
{
    if (is_split_vertex)
    {
        m_collapsedIndices = m_primitiveIndices;
        // index 反向 也不做了
        /*int prim_count = m_collapsedIndices.size() / 3;
        for (int p = 0; p < prim_count; p++)
        {
            int t = m_collapsedIndices[p * 3 + 1];
            m_collapsedIndices[p * 3 + 1] = m_collapsedIndices[p * 3 + 2];
            m_collapsedIndices[p * 3 + 2] = t;
        }*/
        return;
    }
    int prim_set_count = static_cast<int>(m_primitiveIndices.size()) / 3;
    m_collapsedIndices.resize(prim_set_count);
    for (int p = 0; p < prim_set_count; p++)
    {
        m_collapsedIndices[p] = m_primitiveIndices[p * 3 + pos_offset];
    }
    /*int prim_count = m_collapsedIndices.size() / 3;
    // index 反向
    for (int p = 0; p < prim_count; p++)
    {
        int t = m_collapsedIndices[p * 3 + 1];
        m_collapsedIndices[p * 3 + 1] = m_collapsedIndices[p * 3 + 2];
        m_collapsedIndices[p * 3 + 2] = t;
    }*/
}

GenericDto DaeParser::ComposeTriangleListDto(const std::string& geo_name, bool is_skin)
{
    TriangleListDto geo_dto;
    std::string vtx_fmt = "xyz_nor_tex1(2)";
    if (is_skin)
    {
        vtx_fmt = "xyzb5_nor_tex1(2)_betabyte";
    }
    geo_dto.Name() = geo_name;
    geo_dto.VertexFormat() = vtx_fmt;
    geo_dto.Position3s() = m_splitedPositions;
    geo_dto.Normals() = m_collapsedNormals;
    geo_dto.Indices() = m_collapsedIndices;
    geo_dto.VertexCapacity() = static_cast<unsigned>(m_splitedPositions.size());
    geo_dto.IndexCapacity() = static_cast<unsigned>(m_collapsedIndices.size());
    geo_dto.VertexUsedCount() = static_cast<unsigned>(m_splitedPositions.size());
    geo_dto.IndexUsedCount() = static_cast<unsigned>(m_collapsedIndices.size());
    geo_dto.Segments() = { 0, static_cast<unsigned>(m_splitedPositions.size()), 0, static_cast<unsigned>(m_collapsedIndices.size()) };
    geo_dto.Topology() = static_cast<unsigned>(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    if (is_skin)
    {
        geo_dto.PaletteIndices() = m_splitedWeightIndices;
        geo_dto.Weights() = m_splitedVertexWeights;
    }
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = m_splitedTexCoord[0];
    geo_dto.TextureCoords().emplace_back(tex_dto.toGenericDto());
    Box3 box = ContainmentBox3::ComputeAlignedBox(&m_splitedPositions[0], static_cast<unsigned>(m_splitedPositions.size()));
    BoundingVolume bounding = BoundingVolume{ box };
    geo_dto.GeometryBound() = bounding.serializeDto().toGenericDto();
    auto generic_dto = geo_dto.toGenericDto();
    auto filename = Filename("pawns/" + geo_name + ".geo@APK_PATH");
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto> { generic_dto });
    IFilePtr iFile = FileSystem::Instance()->OpenFile(filename, Write | OpenAlways | Binary);
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    FactoryDesc desc = generic_dto.GetRtti();
    desc.ClaimFromResource(geo_name, filename.GetSubPathFileName(), "APK_PATH");
    generic_dto.AddRtti(desc);
    return generic_dto;
}

EffectMaterialDto DaeParser::MakeMaterialDto(const std::string& eff_name, const std::string& eff_filename)
{
    EffectMaterialDto dto;
    dto.Name() = eff_name;
    dto.factoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.getName()).ClaimFromResource(eff_name, eff_filename, "APK_PATH");
    return dto;
}

EffectTextureMapDto DaeParser::MakeEffectTextureMapDto(const std::string& filename, const std::string& tex_name, const std::string& semantic)
{
    EffectTextureMapDto dto;
    TextureMappingDto tex;
    tex.JobType() = TexturePolicy::JobType::Load;
    tex.Filename() = filename;
    tex.Semantic() = semantic;
    tex.TextureName() = tex_name;
    tex.PathId() = "APK_PATH";
    dto.TextureMappings().emplace_back(tex);
    return dto;
}

pugi::xml_node DaeParser::FindNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id)
{
    return node_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((token_name == node.name()) && (node.attribute(TOKEN_ID))
                && (id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
}

std::string DaeParser::FindMaterialTexImageFilename(const pugi::xml_node& collada_root, const std::string& material_id)
{
    if (!collada_root) return "";
    if (material_id.length() == 0) return "";
    pugi::xml_node material_node = FindNodeWithId(collada_root, TOKEN_MATERIAL, material_id);
    if (!material_node)
    {
        OutputLog("can't find material " + material_id);
        return "";
    }
    pugi::xml_node effect_inst_node = material_node.child(TOKEN_INSTANCE_EFFECT);
    if (!effect_inst_node)
    {
        OutputLog("no instance effect in material");
        return "";
    }
    std::string fx_url = "";
    if (effect_inst_node.attribute(TOKEN_URL)) fx_url = &(effect_inst_node.attribute(TOKEN_URL).as_string()[1]);
    if (fx_url == "")
    {
        OutputLog("no fx url define");
        return "";
    }
    pugi::xml_node effect_node = FindNodeWithId(collada_root, TOKEN_EFFECT, fx_url);
    if (!effect_node)
    {
        OutputLog("can't find effect " + fx_url);
        return "";
    }
    pugi::xml_node diffuse_node = effect_node.find_node([=](pugi::xml_node node) -> bool
        { return (strcmp(node.name(), TOKEN_DIFFUSE) == 0); });
    if (!diffuse_node)
    {
        OutputLog("no diffuse node define");
        return "";
    }
    pugi::xml_node texture_node = diffuse_node.child(TOKEN_TEXTURE);
    if (!texture_node)
    {
        OutputLog("no texture node define");
        return "";
    }
    std::string image_or_sampler_id = "";
    if (texture_node.attribute(TOKEN_TEXTURE)) image_or_sampler_id = texture_node.attribute(TOKEN_TEXTURE).as_string();
    if (image_or_sampler_id == "")
    {
        OutputLog("no image id define");
        return "";
    }
    pugi::xml_node image_node;
    if (image_or_sampler_id.rfind(SUFFIX_IMAGE) == image_or_sampler_id.length() - std::string{ SUFFIX_IMAGE }.length())
    {
        image_node = FindNodeWithId(collada_root, TOKEN_IMAGE, image_or_sampler_id);
    }
    else if (image_or_sampler_id.rfind(SUFFIX_SAMPLER)
        == image_or_sampler_id.length() - std::string{ SUFFIX_SAMPLER }.length())
    {
        pugi::xml_node surface_node = effect_node.find_node([=](pugi::xml_node node) -> bool
            { return (strcmp(node.name(), TOKEN_SURFACE) == 0); });
        if (!surface_node)
        {
            OutputLog("can't find sampler surface node");
            return "";
        }
        pugi::xml_node surface_init_from = surface_node.child(TOKEN_INIT_FROM);
        if (!surface_init_from)
        {
            OutputLog("surface no init_from node!!");
            return "";
        }
        image_node = FindNodeWithId(collada_root, TOKEN_IMAGE, surface_init_from.text().as_string());
    }
    if (!image_node)
    {
        OutputLog("can't find image " + image_or_sampler_id);
        return "";
    }
    pugi::xml_node init_from = image_node.child(TOKEN_INIT_FROM);
    if (!init_from)
    {
        OutputLog("image no init_from node!!");
        return "";
    }
    std::string file_url = init_from.text().as_string();
    std::size_t pos = file_url.find_first_of("//");
    Filename filename;
    if (pos != std::string::npos)
    {
        filename = Filename(file_url.substr(pos + 2));
    }
    else
    {
        filename = Filename(file_url);
    }
    OutputLog("    " + material_id + " image file name: " + filename.GetFileName());
    return "image/" + filename.GetFileName();
}

void DaeParser::ClearParsedDatas()
{
    m_positions.clear();
    m_normals.clear();
    m_texcoords[0].clear();
    m_texcoords[1].clear();
    m_texcoords[2].clear();
    m_texcoords[3].clear();
    m_primitiveIndices.clear();
    m_collapsedIndices.clear();
    m_splitedPositions.clear();
    m_collapsedNormals.clear();
    m_splitedTexCoord[0].clear();
    m_splitedTexCoord[1].clear();
    m_splitedTexCoord[2].clear();
    m_splitedTexCoord[3].clear();
    m_weightCounts.clear();
    m_weightPaletteIndices.clear();
    m_splitedWeightIndices.clear();
    m_vertexWeights.clear();
    m_splitedVertexWeights.clear();
    m_weightSource.clear();
    //m_skinBoneNames.clear();
    m_timeValues.clear();
    m_animMatrixs.clear();
}
