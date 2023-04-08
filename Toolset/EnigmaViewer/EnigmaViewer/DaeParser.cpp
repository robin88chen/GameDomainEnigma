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
#include <sstream>

using namespace EnigmaViewer;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::SceneGraph;

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

#define DEFAULT_COLOR_MESH_EFFECT_FILENAME "fx/default_color_mesh_effect.efx"
#define DEFAULT_COLOR_MESH_EFFECT_NAME "default_color_mesh_effect"
#define DEFAULT_TEXTURED_MESH_EFFECT_FILENAME "fx/default_textured_mesh_effect.efx"
#define DEFAULT_TEXTURED_MESH_EFFECT_NAME "default_textured_mesh_effect"
#define DEFAULT_TEXTURED_SKIN_MESH_EFFECT_FILENAME "fx/default_textured_skinmesh_effect.efx"
#define DIFFUSE_MAP_SEMANTIC "DiffuseMap"

#define MAX_WEIGHT_COUNT 4

DaeParser::DaeParser(const std::shared_ptr<Enigma::Engine::GeometryRepository>& geometry_repository)
{
    m_geometryRepository = geometry_repository;
}

DaeParser::~DaeParser()
{

}

void DaeParser::LoadDaeFile(const std::string& filename)
{
    IFilePtr file = FileSystem::Instance()->OpenFile(filename, "rb", "");
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
    m_pawn = PawnDto();
    ParseScene(collada_root);
    /*ParseAnimations(collada_root);

    if ((m_model) && (m_animation))
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
    CommandBus::Post(std::make_shared<OutputMessage>(msg));
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
    ModelPrimitiveDto model_dto;
    model_dto.Name() = model_scene_node.attribute(TOKEN_NAME).as_string();
    m_modelName = model_dto.Name();
    MeshNodeTreeDto tree_dto;
    //m_model = std::make_shared<ModelPrimitive>(model_scene_node.attribute(TOKEN_NAME).as_string());
    pugi::xml_node scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNode(tree_dto, scene_node_xml, std::nullopt);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }
    model_dto.TheNodeTree() = tree_dto.ToGenericDto();

    BoundingVolume unit_bv(Box3::UNIT_BOX);
    m_pawn.ThePrimitive() = model_dto.ToGenericDto();
    m_pawn.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    m_pawn.Name() = m_filename;
    m_pawn.LocalTransform() = Matrix4::IDENTITY;
    m_pawn.WorldTransform() = Matrix4::IDENTITY;
    m_pawn.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    m_pawn.WorldBound() = unit_bv.SerializeDto().ToGenericDto();
    m_pawn.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    // parse for skin
    /*scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNodeForSkin(scene_node_xml);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }*/

    //m_model->SelectVisualTechnique("Default");
}

void DaeParser::ParseSceneNode(Enigma::Renderer::MeshNodeTreeDto& node_tree, const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index)
{
    std::string node_id = std::string(scene_node_xml.attribute(TOKEN_ID).as_string());
    OutputLog("   Parse scene node " + node_id + "...");
    pugi::xml_node inst_controller = scene_node_xml.child(TOKEN_INSTANCE_CONTROLLER);
    if (inst_controller)
    {
        OutputLog("   " + node_id + " is a skin mesh node");
        //ParseSceneNodeForSkin(scene_node_xml, parent_node_array_index);
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
        mesh_node.LocalTransform() = mx;
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
    node_tree.MeshNodes().emplace_back(mesh_node.ToGenericDto());
    pugi::xml_node child_node = scene_node_xml.child(TOKEN_NODE);
    while (child_node)
    {
        ParseSceneNode(node_tree, child_node, current_index);
        child_node = child_node.next_sibling(TOKEN_NODE);
    }
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
            dto.Effects().emplace_back(MakeMaterialDto(DEFAULT_COLOR_MESH_EFFECT_NAME, DEFAULT_COLOR_MESH_EFFECT_FILENAME).ToGenericDto());
        }
        else
        {
            dto.Effects().emplace_back(MakeMaterialDto(DEFAULT_TEXTURED_MESH_EFFECT_NAME, DEFAULT_TEXTURED_MESH_EFFECT_FILENAME).ToGenericDto());
            dto.TextureMaps().emplace_back(MakeEffectTextureMapDto(texmap_filename, texmap_filename, DIFFUSE_MAP_SEMANTIC).ToGenericDto());
        }
        mesh_node.TheMeshPrimitive() = dto.ToGenericDto();
    }
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
    geo_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    Box3 box = ContainmentBox3::ComputeAlignedBox(&m_splitedPositions[0], static_cast<unsigned>(m_splitedPositions.size()));
    BoundingVolume bounding = BoundingVolume{ box };
    geo_dto.GeometryBound() = bounding.SerializeDto().ToGenericDto();
    auto generic_dto = geo_dto.ToGenericDto();
    auto filename = Filename("pawns/" + geo_name + ".geo@APK_PATH");
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto> { generic_dto });
    IFilePtr iFile = FileSystem::Instance()->OpenFile(filename, "w+b");
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
    dto.TheFactoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.GetName()).ClaimFromResource(eff_name, eff_filename, "APK_PATH");
    return dto;
}

EffectTextureMapDto DaeParser::MakeEffectTextureMapDto(const std::string& filename, const std::string& tex_name, const std::string& semantic)
{
    EffectTextureMapDto dto;
    TextureMappingDto tex;
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
    /*m_timeValues.clear();
    m_animMatrixs.clear();*/
}
