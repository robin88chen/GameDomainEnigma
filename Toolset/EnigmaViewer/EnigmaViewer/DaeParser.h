/*********************************************************************
 * \file   DaeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _DAE_PARSER_H
#define _DAE_PARSER_H

#include "Animators/AnimationAssetAssembler.h"
#include "GameCommon/AnimatedPawnAssembler.h"
#include "Geometries/GeometryData.h"
#include "Geometries/GeometryRepository.h"
#include "DaeParserConfiguration.h"
#include "Renderables/MeshNodeAssemblers.h"
#include "Renderables/ModelAnimationAssembler.h"
#include "Renderables/ModelPrimitiveAssembler.h"
#include "Animators/AnimationAssetStoreMapper.h"
#include "Animators/AnimatorStoreMapper.h"
#include "Primitives/PrimitiveStoreMapper.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/TextureId.h"
#include "pugixml.hpp"
#include <string>
#include <optional>

namespace EnigmaViewer
{
    class DaeParser
    {
    public:
        DaeParser(const std::shared_ptr<Enigma::Geometries::GeometryDataStoreMapper>& geometry_store_mapper, const std::shared_ptr<Enigma::Animators::AnimationAssetStoreMapper>& animation_asset_store_mapper, const std::shared_ptr<Enigma::Animators::AnimatorStoreMapper>& animator_store_mapper, const std::shared_ptr<Enigma::Primitives::PrimitiveStoreMapper>& primitive_store_mapper);
        virtual ~DaeParser();

        void loadDaeFile(const std::string& filename);
        Enigma::Primitives::PrimitiveId getModelId() const { return m_modelId; }

        /*private:
            struct GeometryValueOffsets
            {
                int pos_offset;
                int nor_offset;
                int tex_offset[4];
            };*/
    private:
        void outputLog(const std::string& msg);

        //void parseScene(const pugi::xml_node& collada_root);
        //void parseModelSceneNode(const pugi::xml_node& model_scene_node);
        //void parseSceneNode(const std::shared_ptr<Enigma::Renderables::MeshNodeTreeAssembler>& node_tree_assembler, const pugi::xml_node& scene_node_xml, std::optional<std::string> parent_node_name);

        //void parseGeometryInstanceNode(const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& mesh_node_assembler, const pugi::xml_node& geometry_inst);
        //void parseSingleGeometry(const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& mesh_node_assembler, const pugi::xml_node& geometry_xml_node, bool is_skin);

        //void parseSceneNodeForSkin(const std::shared_ptr<Enigma::Renderables::MeshNodeTreeAssembler>& node_tree_assembler, const pugi::xml_node& scene_node_xml, std::optional<std::string> parent_node_name);
        //void parseSkinMesh(const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& mesh_node_assembler, const pugi::xml_node& skin_node_xml);
        //void parseVertexWeights(const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& mesh_node_assembler, const pugi::xml_node& skin_host_xml, const pugi::xml_node& vertex_weights_xml);
        //void parseJointNameSource(const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& mesh_node_assembler, const pugi::xml_node& bone_names_xml);
        //void parseWeightsArraySource(const pugi::xml_node& weights_array_xml);

        //std::tuple<std::string, GeometryValueOffsets> parseGeometryMesh(const pugi::xml_node& mesh_node);
        //void parsePositionSource(const pugi::xml_node& position_source);
        //void parseNormalSource(const pugi::xml_node& normal_source);
        //void parseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set);
        //void parseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count);

        //void parseAnimations(const pugi::xml_node& collada_root);
        //void parseSingleAnimation(const std::shared_ptr<Enigma::Renderables::ModelAnimationAssembler>& animation_assembler, const pugi::xml_node& anim_node);
        //void parseAnimationSample(const std::shared_ptr<Enigma::Renderables::AnimationTimeSRTAssembler>& srt_assembler, const pugi::xml_node& sampler_node,
          //  const pugi::xml_node& anim_node);
        //void parseTimeValue(const pugi::xml_node& time_value_source);
        //void parseAnimationMatrix(const pugi::xml_node& anim_matrix_source);
        //void analyzeSRTKeys(const std::shared_ptr<Enigma::Renderables::AnimationTimeSRTAssembler>& srt_assembler);

        //void splitVertexPositions(int pos_offset, int tex_set, int tex_offset, bool is_split_vertex, bool is_skin);
        //void collapseVertexNormals(int pos_offset, int nor_offset, bool is_split_vertex);
        //void collapseIndexArray(int pos_offset, bool is_split_vertex);

        //pugi::xml_node findNodeWithId(const pugi::xml_node& node_root, const std::string& token_name,
          //  const std::string& id);
        //std::string findMaterialTexImageFilename(const pugi::xml_node& collada_root, const std::string& material_id);
        //void clearParsedData();

        //void persistSingleGeometry(const Enigma::Geometries::GeometryId& geo_id, bool is_skin);
        //void persistMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic);
        //void persistSkinMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic);
        //void persistAnimator(const Enigma::Animators::AnimatorId& animator_id, const Enigma::Animators::AnimationAssetId& animation_asset_id);
        //void persistModel(const Enigma::Animators::AnimatorId& animator_id);

        //std::optional<Enigma::Primitives::PrimitiveId> meshIdInMeshNode(const std::string& name);

    private:
        DaeParserConfiguration m_config;
        std::weak_ptr<Enigma::Geometries::GeometryDataStoreMapper> m_geometryStoreMapper;
        std::weak_ptr<Enigma::Animators::AnimationAssetStoreMapper> m_animationStoreMapper;
        std::weak_ptr<Enigma::Animators::AnimatorStoreMapper> m_animatorStoreMapper;
        std::weak_ptr<Enigma::Primitives::PrimitiveStoreMapper> m_primitiveStoreMapper;

        std::string m_filename;
        Enigma::Primitives::PrimitiveId m_modelId;
        std::shared_ptr<Enigma::Renderables::ModelPrimitiveAssembler> m_modelAssembler;
        //Enigma::Animators::AnimationAssetId m_animationAssetId;
        //Enigma::Animators::AnimatorId m_animatorId;
        std::string m_modelName;

        //std::vector<Enigma::MathLib::Vector3> m_positions;
        //std::vector<Enigma::MathLib::Vector3> m_normals;
        //std::vector<Enigma::MathLib::Vector2> m_texcoords[4];
        //std::vector<unsigned> m_primitiveIndices;
        //std::vector<unsigned> m_collapsedIndices;
        //std::vector<Enigma::MathLib::Vector3> m_splitedPositions;
        //std::vector<Enigma::MathLib::Vector3> m_collapsedNormals;
        //std::vector<Enigma::MathLib::Vector2> m_splitedTexCoord[4];

        //std::vector<int> m_weightCounts;
        //std::vector<unsigned int> m_weightPaletteIndices;
        //std::vector<unsigned int> m_splitedWeightIndices;
        //std::vector<float> m_vertexWeights;
        //std::vector<float> m_splitedVertexWeights;
        //std::vector<float> m_weightSource;
        //std::unordered_map<std::string, std::vector<std::string>> m_skinBoneNames;
        //std::unordered_map<std::string, Enigma::Primitives::PrimitiveId> m_meshIdInMeshNode;

        //std::vector<float> m_timeValues;
        //std::vector<Enigma::MathLib::Matrix4> m_animMatrixs;

        //std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
        //std::unordered_map<std::string, std::string> m_nodeJointIdMapping;
    };
}
#endif // DAEPARSER_H
