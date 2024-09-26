#include "DaeSceneNodeParser.h"
#include "DaeParserErrors.h"
#include "DaeGeometryParser.h"
#include "DaeSchema.h"
#include "FileSystem/Filename.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/MeshPrimitiveAssembler.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "ViewerCommands.h"
#include <cassert>
#include <sstream>

using namespace EnigmaViewer;

#define TOKEN_MATRIX    "matrix"
#define TOKEN_INSTANCE_GEOMETRY "instance_geometry"
#define TOKEN_MATERIAL "material"
#define TOKEN_EFFECT "effect"
#define TOKEN_IMAGE "image"
#define TOKEN_INSTANCE_EFFECT "instance_effect"
#define TOKEN_URL "url"
#define TOKEN_DIFFUSE "diffuse"
#define TOKEN_TEXTURE "texture"
#define SUFFIX_IMAGE "-image"
#define SUFFIX_SAMPLER "-sampler"
#define TOKEN_SURFACE "surface"
#define TOKEN_INIT_FROM "init_from"

#define DIFFUSE_MAP_SEMANTIC "DiffuseMap"

DaeSceneNodeParser::DaeSceneNodeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name)
    : m_outputPipe(output_pipe), m_modelName(model_name)
{
    assert(m_outputPipe);
    m_config.loadConfig();
}

std::error_code DaeSceneNodeParser::parseSceneNode(const pugi::xml_node& scene_node_xml, const std::string& mesh_node_name)
{
    m_nodeAssembler = std::make_shared<Enigma::Renderables::MeshNodeAssembler>();
    m_nodeAssembler->name(mesh_node_name);
    pugi::xml_node matrix_xml_node = scene_node_xml.child(TOKEN_MATRIX);
    if (matrix_xml_node)
    {
        Enigma::MathLib::Matrix4 mx;
        std::stringstream ss(std::string(matrix_xml_node.text().as_string()));
        for (int i = 0; i < 16; i++)
        {
            ss >> ((float*)mx)[i];
        }
        m_nodeAssembler->localT_PosTransform(mx);
    }
    else
    {
        return ParserError::noMatrixTransform; // this is not a mesh node
    }
    pugi::xml_node geometry_inst = scene_node_xml.child(TOKEN_INSTANCE_GEOMETRY);
    if (geometry_inst)
    {
        auto er = parseGeometryInstanceNode(geometry_inst);
        if (er) return er;
    }
    return ParserError::ok;
}

std::error_code DaeSceneNodeParser::parseGeometryInstanceNode(const pugi::xml_node& geometry_inst)
{
    DaeGeometryParser geometry_parser(m_outputPipe);
    std::error_code er = geometry_parser.parseGeometryInstanceNode(geometry_inst, m_modelName);
    if (er)
    {
        m_outputPipe("parse geometry instance node error!!");
        return er;
    }

    std::string texmap_filename = "";
    std::string material_id = geometry_parser.getMeshMaterialId();
    if (material_id.length() > 0)
    {
        texmap_filename = findMaterialTexImageFilename(geometry_inst.root(), material_id);
    }
    std::string geo_name = geometry_parser.getGeometryName();
    Enigma::Geometries::GeometryId geo_id = geometry_parser.getGeometryId();

    Enigma::Primitives::PrimitiveId mesh_id("renderables/" + geo_name, Enigma::Renderables::MeshPrimitive::TYPE_RTTI);
    DaeSchema::addMeshIdInMeshNodeName(m_nodeAssembler->name(), mesh_id);
    if (texmap_filename.empty())
    {
        persistMesh(mesh_id, geo_id, Enigma::Engine::EffectMaterialId(m_config.defaultColorMeshEffectName()), std::nullopt, std::nullopt);
    }
    else
    {
        persistMesh(mesh_id, geo_id, Enigma::Engine::EffectMaterialId(m_config.defaultTexturedMeshEffectName()), Enigma::Engine::TextureId(texmap_filename), DIFFUSE_MAP_SEMANTIC);
    }
    m_nodeAssembler->meshPrimitiveId(mesh_id);
    return er;
}

std::string DaeSceneNodeParser::findMaterialTexImageFilename(const pugi::xml_node& collada_root, const std::string& material_id)
{
    if (!collada_root) return "";
    if (material_id.length() == 0) return "";
    pugi::xml_node material_node = DaeSchema::findChildNodeWithId(collada_root, TOKEN_MATERIAL, material_id);
    if (!material_node)
    {
        m_outputPipe("can't find material " + material_id);
        return "";
    }
    pugi::xml_node effect_inst_node = material_node.child(TOKEN_INSTANCE_EFFECT);
    if (!effect_inst_node)
    {
        m_outputPipe("no instance effect in material");
        return "";
    }
    std::string fx_url = "";
    if (effect_inst_node.attribute(TOKEN_URL)) fx_url = &(effect_inst_node.attribute(TOKEN_URL).as_string()[1]);
    if (fx_url == "")
    {
        m_outputPipe("no fx url define");
        return "";
    }
    pugi::xml_node effect_node = DaeSchema::findChildNodeWithId(collada_root, TOKEN_EFFECT, fx_url);
    if (!effect_node)
    {
        m_outputPipe("can't find effect " + fx_url);
        return "";
    }
    pugi::xml_node diffuse_node = effect_node.find_node([=](pugi::xml_node node) -> bool
        { return (strcmp(node.name(), TOKEN_DIFFUSE) == 0); });
    if (!diffuse_node)
    {
        m_outputPipe("no diffuse node define");
        return "";
    }
    pugi::xml_node texture_node = diffuse_node.child(TOKEN_TEXTURE);
    if (!texture_node)
    {
        m_outputPipe("no texture node define");
        return "";
    }
    std::string image_or_sampler_id = "";
    if (texture_node.attribute(TOKEN_TEXTURE)) image_or_sampler_id = texture_node.attribute(TOKEN_TEXTURE).as_string();
    if (image_or_sampler_id == "")
    {
        m_outputPipe("no image id define");
        return "";
    }
    pugi::xml_node image_node;
    if (image_or_sampler_id.rfind(SUFFIX_IMAGE) == image_or_sampler_id.length() - std::string{ SUFFIX_IMAGE }.length())
    {
        image_node = DaeSchema::findChildNodeWithId(collada_root, TOKEN_IMAGE, image_or_sampler_id);
    }
    else if (image_or_sampler_id.rfind(SUFFIX_SAMPLER)
        == image_or_sampler_id.length() - std::string{ SUFFIX_SAMPLER }.length())
    {
        pugi::xml_node surface_node = effect_node.find_node([=](pugi::xml_node node) -> bool
            { return (strcmp(node.name(), TOKEN_SURFACE) == 0); });
        if (!surface_node)
        {
            m_outputPipe("can't find sampler surface node");
            return "";
        }
        pugi::xml_node surface_init_from = surface_node.child(TOKEN_INIT_FROM);
        if (!surface_init_from)
        {
            m_outputPipe("surface no init_from node!!");
            return "";
        }
        image_node = DaeSchema::findChildNodeWithId(collada_root, TOKEN_IMAGE, surface_init_from.text().as_string());
    }
    if (!image_node)
    {
        m_outputPipe("can't find image " + image_or_sampler_id);
        return "";
    }
    pugi::xml_node init_from = image_node.child(TOKEN_INIT_FROM);
    if (!init_from)
    {
        m_outputPipe("image no init_from node!!");
        return "";
    }
    std::string file_url = init_from.text().as_string();
    std::size_t pos = file_url.find_first_of("//");
    Enigma::FileSystem::Filename filename;
    if (pos != std::string::npos)
    {
        filename = Enigma::FileSystem::Filename(file_url.substr(pos + 2));
    }
    else
    {
        filename = Enigma::FileSystem::Filename(file_url);
    }
    m_outputPipe("    " + material_id + " image file name: " + filename.getFileName());
    return "image/" + filename.getBaseFileName();
}

void DaeSceneNodeParser::persistMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic)
{
    Enigma::Renderables::MeshPrimitiveAssembler mesh_assembler(mesh_id);
    mesh_assembler.geometryId(geo_id);
    mesh_assembler.asNative(mesh_id.name() + ".mesh@APK_PATH");
    mesh_assembler.addEffect(effect_id);
    if ((texture_id) && (tex_semantic))
    {
        std::shared_ptr<Enigma::Engine::EffectTextureMapAssembler> texture_assembler = std::make_shared<Enigma::Engine::EffectTextureMapAssembler>();
        texture_assembler->addTextureMapping(texture_id.value(), std::nullopt, tex_semantic.value());
        mesh_assembler.addTextureMap(texture_assembler);
    }
    mesh_assembler.renderListID(Enigma::Renderer::Renderer::RenderListID::Scene);
    mesh_assembler.visualTechnique("Default");
    std::make_shared<PersistPrimitiveDto>(mesh_id, mesh_assembler.assemble())->execute();
}

