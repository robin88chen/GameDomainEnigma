#include "DaeSkinSceneNodeParser.h"
#include "Renderables/SkinMeshPrimitive.h"
#include "DaeGeometryParser.h"
#include "Renderables/SkinMeshPrimitiveAssembler.h"
#include "DaeSchema.h"
#include "ViewerCommands.h"

using namespace EnigmaViewer;

#define DIFFUSE_MAP_SEMANTIC "DiffuseMap"

DaeSkinSceneNodeParser::DaeSkinSceneNodeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name) : DaeSceneNodeParser(output_pipe, model_name)
{
}

std::error_code DaeSkinSceneNodeParser::parseSkinSceneNode(const pugi::xml_node& skin_scene_node)
{
    DaeGeometryParser geometry_parser(m_outputPipe);
    std::error_code er = geometry_parser.parseSceneNodeForSkin(skin_scene_node, m_modelName);
    if (er)
    {
        m_outputPipe("parse skin mesh node error!!");
        return er;
    }
    std::string mesh_node_name = geometry_parser.getSkinMeshNodeName();
    m_skinBoneNames = geometry_parser.getSkinBoneNames();
    m_nodeAssembler = std::make_shared<Enigma::Renderables::MeshNodeAssembler>();
    m_nodeAssembler->name(mesh_node_name);
    m_nodeAssembler->localT_PosTransform(geometry_parser.getBindShapeMatrix());

    std::string texmap_filename = "";
    std::string material_id = geometry_parser.getMeshMaterialId();
    if (material_id.length() > 0)
    {
        texmap_filename = findMaterialTexImageFilename(skin_scene_node.root(), material_id);
    }
    std::string geo_name = geometry_parser.getGeometryName();
    Enigma::Geometries::GeometryId geo_id = geometry_parser.getGeometryId();
    Enigma::Primitives::PrimitiveId mesh_id("renderables/" + geo_name, Enigma::Renderables::SkinMeshPrimitive::TYPE_RTTI);
    DaeSchema::addMeshIdInMeshNodeName(mesh_node_name, mesh_id);
    if (texmap_filename.empty())
    {
        persistMesh(mesh_id, geo_id, Enigma::Engine::EffectMaterialId(m_config.defaultColorMeshEffectName()), std::nullopt, std::nullopt);
    }
    else
    {
        persistMesh(mesh_id, geo_id, Enigma::Engine::EffectMaterialId(m_config.defaultTexturedSkinMeshEffectName()), Enigma::Engine::TextureId(texmap_filename), DIFFUSE_MAP_SEMANTIC);
    }
    m_nodeAssembler->meshPrimitiveId(mesh_id);
    return er;
}

void DaeSkinSceneNodeParser::persistMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic)
{
    Enigma::Renderables::SkinMeshPrimitiveAssembler mesh_assembler(mesh_id);
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

