#include "DaeParser.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "ViewerCommands.h"
#include "Frameworks/CommandBus.h"
#include "Renderer/ModelPrimitive.h"
#include "GameEngine/GeometryDataDto.h"
#include <sstream>

using namespace EnigmaViewer;
using namespace Enigma::FileSystem;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

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
    unsigned int file_size = file->Size();
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
    m_model = nullptr;
    ParseModelSceneNode(model_scene_node);
}

void DaeParser::ParseModelSceneNode(const pugi::xml_node& model_scene_node)
{
    OutputLog(" Parse model scene node "
        + std::string(model_scene_node.attribute(TOKEN_ID).as_string()) + "...");
    m_model = std::make_shared<ModelPrimitive>(model_scene_node.attribute(TOKEN_NAME).as_string());
    pugi::xml_node scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNode(scene_node_xml, std::nullopt);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }
    // parse for skin
    /*scene_node_xml = model_scene_node.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        ParseSceneNodeForSkin(scene_node_xml);
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }*/

    m_model->SelectVisualTechnique("Default");
}

void DaeParser::ParseSceneNode(const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index)
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

    MeshNode mesh_node = MeshNode(mesh_node_name);
    pugi::xml_node matrix_xml_node = scene_node_xml.child(TOKEN_MATRIX);
    if (matrix_xml_node)
    {
        Matrix4 mx;
        std::stringstream ss(std::string(matrix_xml_node.text().as_string()));
        for (int i = 0; i < 16; i++)
        {
            ss >> ((float*)mx)[i];
        }
        mesh_node.SetLocalTransform(mx);
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
    if (parent_node_array_index) mesh_node.SetParentIndexInArray(parent_node_array_index.value());
    std::optional<unsigned> current_index;
    if (m_model)
    {
        current_index = m_model->GetMeshNodeTree().AddMeshNode(mesh_node);
    }
    pugi::xml_node child_node = scene_node_xml.child(TOKEN_NODE);
    while (child_node)
    {
        ParseSceneNode(child_node, current_index);
        child_node = child_node.next_sibling(TOKEN_NODE);
    }
}

void DaeParser::ParseGeometryInstanceNode(MeshNode& mesh_node, const pugi::xml_node& geometry_inst)
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

void DaeParser::ParseSingleGeometry(MeshNode& mesh_node, const pugi::xml_node& geometry_node, bool is_skin)
{
    pugi::xml_attribute attr_name = geometry_node.attribute(TOKEN_NAME);
    if (!attr_name)
    {
        OutputLog("geometry do not have name!!");
    }
    std::string geo_name;
    if (m_model)
    {
        geo_name = m_model->GetName() + attr_name.as_string();
    }
    else
    {
        geo_name = attr_name.as_string();
    }
    TriangleListDto geo_dto;
    OutputLog("  Parse Geometry " + std::string(attr_name.as_string()));
    int pos_offset, nor_offset = -1, tex_offset[4];
    std::string material_id = "";
    memset(tex_offset, 0xff, sizeof(tex_offset));
    ParseGeometryMesh(geometry_node.child(TOKEN_MESH), material_id, pos_offset, nor_offset, tex_offset);
    /*bool is_split_vertex = true;
    if (nor_offset >= 0)
    {
        is_split_vertex = false;
    }
    for (int i = 0; i < 4; i++)
    {
        if (tex_offset[i] >= 0) is_split_vertex = false;
    }
    std::string texmap_filename = "";
    if (material_id.length() > 0)
    {
        texmap_filename = FindMaterialTexImageFilename(geometry_node.root(), material_id);
    }
    SplitVertexPositions(pos_offset, 0, tex_offset[0], is_split_vertex, is_skin);
    CollapseVertexNormals(pos_offset, nor_offset, is_split_vertex);
    CollapseIndexArray(pos_offset, is_split_vertex);
    if (geo)
    {
        std::string vtx_fmt = "xyz_nor_tex1(2)";
        if (is_skin)
        {
            vtx_fmt = "xyzb5_nor_tex1(2)_betabyte";
        }
        geo->CreateVertexCapacity(vtx_fmt, m_splitedPositions.size(), m_splitedPositions.size(),
            m_collapsedIndices.size(), m_collapsedIndices.size());
        geo->SetPositionArray(&m_splitedPositions[0], m_splitedPositions.size());
        geo->SetVertexNormalArray(&m_collapsedNormals[0], m_collapsedNormals.size());
        geo->SetTexture2DCoordArray(0, &m_splitedTexCoord[0][0], m_splitedTexCoord[0].size());
        geo->SetIndexArray((unsigned int*)&m_collapsedIndices[0], m_collapsedIndices.size());
        if (is_skin)
        {
            geo->SetPaletteIndexArray(&m_splitedWeightIndices[0], m_splitedWeightIndices.size());
            geo->SetTotalSkinWeightArray(&m_splitedVertexWeights[0], m_splitedVertexWeights.size());
        }
        geo->SetDataStatus(Object::DataStatus::Ready);
        geo->GetFactoryDesc().ClaimAsResourceAsset(geo_name, geo_name + ".geo");

        Enigma::SkinMeshPrimitivePtr skin_mesh = nullptr;
        Enigma::MeshPrimitivePtr geo_mesh = nullptr;
        if (is_skin)
        {
            skin_mesh = SkinMeshPrimitivePtr{ menew SkinMeshPrimitive() };
            geo_mesh = std::static_pointer_cast<MeshPrimitive>(skin_mesh);
        }
        else
        {
            geo_mesh = MeshPrimitivePtr{ menew MeshPrimitive() };
        }
        geo_mesh->SetMeshName(attr_name.as_string());
        geo_mesh->LinkGeometryData(geo);
        Enigma::EffectMaterialPtr effect = nullptr;
        std::string default_efx_filename = DEFAULT_TEXTURED_MESH_EFFECT_FILENAME;
        if (is_skin)
        {
            default_efx_filename = DEFAULT_TEXTURED_SKIN_MESH_EFFECT_FILENAME;
        }
        if (texmap_filename.length() > 0)
        {
            Enigma::ServiceManager::GetSystemServiceAs<Enigma::EffectMaterialManager>()
                ->AsyncQueryEffectMaterial(default_efx_filename, &effect).wait();
            if (effect)
            {
                effect->GetFactoryDesc().ClaimFromResource(default_efx_filename, default_efx_filename);
            }
            MakeTextureToMesh(geo_mesh, texmap_filename);
        }
        else
        {
            Enigma::ServiceManager::GetSystemServiceAs<Enigma::EffectMaterialManager>()
                ->AsyncQueryEffectMaterial(DEFAULT_COLOR_MESH_EFFECT_FILENAME, &effect).wait();
            if (effect)
            {
                effect->GetFactoryDesc().ClaimFromResource(default_efx_filename, default_efx_filename);
            }
        }
        /*if (is_skin)
        {
            skin_mesh->CreateBoneMatrixArray(m_boneNames.size());
            Enigma::SkinAnimOperatorPtr skin_op = EN_NEW Enigma::SkinAnimOperator();
            skin_op->LinkSkinMeshPrimitive(skin_mesh, m_boneNames);
        }*/
        /*geo_mesh->ChangeEffectMaterial(MeshPrimitive::EffectMaterialList{ effect });
        geo_mesh->CreateRenderElements(true);
        if (mesh_node)
        {
            mesh_node->SetMeshPrimitive(geo_mesh);
        }
        geo_mesh->SetDataStatus(Object::DataStatus::Ready);
    }*/

}

void DaeParser::ParseGeometryMesh(const pugi::xml_node& mesh_node, std::string& material_id, int& pos_offset, int& nor_offset, int tex_offset[4])
{
    if (!mesh_node)
    {
        OutputLog("no mesh node!!");
        return;
    }
    pugi::xml_node triangles_node = mesh_node.child(TOKEN_TRIANGLES);
    if (!triangles_node)
    {
        OutputLog("no triangles define!!");
        return;
    }
    int triangle_count = triangles_node.attribute(TOKEN_COUNT).as_int();
    material_id = "";
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
            pos_offset = input_source.attribute(TOKEN_OFFSET).as_int();
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
            nor_offset = input_source.attribute(TOKEN_OFFSET).as_int();
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
            tex_offset[texcoord_set] = input_source.attribute(TOKEN_OFFSET).as_int();
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

pugi::xml_node DaeParser::FindNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id)
{
    return node_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((token_name == node.name()) && (node.attribute(TOKEN_ID))
                && (id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
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
    /*m_collapsedIndices.clear();
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
    m_animMatrixs.clear();*/
}
