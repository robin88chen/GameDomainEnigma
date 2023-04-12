/*********************************************************************
 * \file   DaeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _DAE_PARSER_H
#define _DAE_PARSER_H

#include "GameEngine/GeometryRepository.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/EffectMaterialDto.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "Animators/ModelAnimationAsset.h"
#include "Animators/AnimationAssetDtos.h"
#include "pugixml.hpp"
#include <string>
#include <optional>

namespace EnigmaViewer
{
    class DaeParser
    {
    public:
        DaeParser(const std::shared_ptr<Enigma::Engine::GeometryRepository>& geometry_repository);
        virtual ~DaeParser();

        void LoadDaeFile(const std::string& filename);

        const Enigma::SceneGraph::PawnDto& GetPawnDto() { return m_pawn; }

    private:
        struct GeometryValueOffsets
        {
            int pos_offset;
            int nor_offset;
            int tex_offset[4];
        };
    private:
        void OutputLog(const std::string& msg);

        void ParseScene(const pugi::xml_node& collada_root);
        void ParseModelSceneNode(const pugi::xml_node& model_scene_node);
        void ParseSceneNode(Enigma::Renderer::MeshNodeTreeDto& node_tree, const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index);

        void ParseGeometryInstanceNode(Enigma::Renderer::MeshNodeDto& mesh_node, const pugi::xml_node& geometry_inst);
        void ParseSingleGeometry(Enigma::Renderer::MeshNodeDto& mesh_node, const pugi::xml_node& geometry_node, bool is_skin);

        void ParseSceneNodeForSkin(Enigma::Renderer::MeshNodeTreeDto& node_tree, const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index);
        void ParseSkinMesh(Enigma::Renderer::MeshNodeDto& mesh_node, const pugi::xml_node& skin_node_xml);
        void ParseVertexWeights(Enigma::Renderer::MeshNodeDto& mesh_node, const pugi::xml_node& skin_host_xml, const pugi::xml_node& vertex_weights_xml);
        void ParseJointNameSource(Enigma::Renderer::MeshNodeDto& mesh_node, const pugi::xml_node& bone_names_xml);
        void ParseWeightsArraySource(const pugi::xml_node& weights_array_xml);

        std::tuple<std::string, GeometryValueOffsets> ParseGeometryMesh(const pugi::xml_node& mesh_node);
        void ParsePositionSource(const pugi::xml_node& position_source);
        void ParseNormalSource(const pugi::xml_node& normal_source);
        void ParseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set);
        void ParseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count);

        void ParseAnimations(const pugi::xml_node& collada_root);
        void ParseSingleAnimation(const pugi::xml_node& anim_node);
        void ParseAnimationSample(Enigma::Animators::AnimationTimeSRTDto& srt_data, const pugi::xml_node& sampler_node,
            const pugi::xml_node& anim_node);
        void ParseTimeValue(const pugi::xml_node& time_value_source);
        void ParseAnimationMatrix(const pugi::xml_node& anim_matrix_source);
        void AnalyzeSRTKeys(Enigma::Animators::AnimationTimeSRTDto& srt_data);

        void SplitVertexPositions(int pos_offset, int tex_set, int tex_offset, bool is_split_vertex, bool is_skin);
        void CollapseVertexNormals(int pos_offset, int nor_offset, bool is_split_vertex);
        void CollapseIndexArray(int pos_offset, bool is_split_vertex);

        pugi::xml_node FindNodeWithId(const pugi::xml_node& node_root, const std::string& token_name,
            const std::string& id);
        std::string FindMaterialTexImageFilename(const pugi::xml_node& collada_root, const std::string& material_id);
        void ClearParsedDatas();

        Enigma::Engine::GenericDto ComposeTriangleListDto(const std::string& geo_name, bool is_skin);
        Enigma::Engine::EffectMaterialDto MakeMaterialDto(const std::string& eff_name, const std::string& eff_filename);
        Enigma::Engine::EffectTextureMapDto MakeEffectTextureMapDto(const std::string& filename, const std::string& tex_name, const std::string& semantic);

        void ComposeModelPrimitiveDto();

    private:
        std::weak_ptr<Enigma::Engine::GeometryRepository> m_geometryRepository;

        std::string m_filename;
        std::string m_modelName;

        Enigma::Renderer::ModelPrimitiveDto m_model;
        Enigma::SceneGraph::PawnDto m_pawn;
        Enigma::Animators::ModelAnimationAssetDto m_animationAsset;

        std::vector<Enigma::MathLib::Vector3> m_positions;
        std::vector<Enigma::MathLib::Vector3> m_normals;
        std::vector<Enigma::MathLib::Vector2> m_texcoords[4];
        std::vector<unsigned> m_primitiveIndices;
        std::vector<unsigned> m_collapsedIndices;
        std::vector<Enigma::MathLib::Vector3> m_splitedPositions;
        std::vector<Enigma::MathLib::Vector3> m_collapsedNormals;
        std::vector<Enigma::MathLib::Vector2> m_splitedTexCoord[4];

        std::vector<int> m_weightCounts;
        std::vector<unsigned int> m_weightPaletteIndices;
        std::vector<unsigned int> m_splitedWeightIndices;
        std::vector<float> m_vertexWeights;
        std::vector<float> m_splitedVertexWeights;
        std::vector<float> m_weightSource;
        std::unordered_map<std::string, std::vector<std::string>> m_skinBoneNames;

        std::vector<float> m_timeValues;
        std::vector<Enigma::MathLib::Matrix4> m_animMatrixs;

        std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
        std::unordered_map<std::string, std::string> m_nodeJointIdMapping;
    };
}
#endif // DAEPARSER_H
