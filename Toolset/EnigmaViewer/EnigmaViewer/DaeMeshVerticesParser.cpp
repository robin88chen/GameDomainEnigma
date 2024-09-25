#include "DaeMeshVerticesParser.h"
#include "DaeVertexWeightsParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include "Geometries/TriangleListAssembler.h"
#include <cassert>
#include <sstream>

using namespace EnigmaViewer;

#define TOKEN_TRIANGLES "triangles"
#define TOKEN_COUNT "count"
#define TOKEN_MATERIAL "material"
#define TOKEN_INPUT "input"
#define TOKEN_SEMANTIC "semantic"
#define TOKEN_VERTEX_SEMANTIC "VERTEX"
#define TOKEN_POSITION_SEMANTIC "POSITION"
#define TOKEN_NORMAL_SEMANTIC "NORMAL"
#define TOKEN_TEXCOORD_SEMANTIC "TEXCOORD"
#define TOKEN_VERTICES "vertices"
#define TOKEN_SOURCE "source"
#define TOKEN_OFFSET "offset"
#define TOKEN_SET "set"
#define TOKEN_INDEX_ARRAY "p"
#define TOKEN_ID "id"
#define TOKEN_FLOAT_ARRAY "float_array"

DaeMeshVerticesParser::DaeMeshVerticesParser(const std::function<void(const std::string&)>& output_pipe)
{
    assert(output_pipe);
    m_outputPipe = output_pipe;
}

std::error_code DaeMeshVerticesParser::parseVertices(const pugi::xml_node& vertices_xml_node)
{
    memset(m_geometryValueOffsets.tex_offset, 0xff, sizeof(m_geometryValueOffsets.tex_offset));
    if (!vertices_xml_node)
    {
        m_outputPipe("no mesh node!!");
        return ParserError::noMeshVertices;
    }
    pugi::xml_node triangles_node = vertices_xml_node.child(TOKEN_TRIANGLES);
    if (!triangles_node)
    {
        m_outputPipe("no triangles define!!");
        return ParserError::noTriangles;
    }
    int triangle_count = triangles_node.attribute(TOKEN_COUNT).as_int();
    if (triangles_node.attribute(TOKEN_MATERIAL))
    {
        m_meshMaterialId = triangles_node.attribute(TOKEN_MATERIAL).as_string();
    }
    pugi::xml_node input_source = triangles_node.child(TOKEN_INPUT);
    while (input_source)
    {
        if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_VERTEX_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            m_geometryValueOffsets.pos_offset = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node vertices_node = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_VERTICES, source_id);
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
                        pugi::xml_node position_source = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_SOURCE, position_source_id);
                        if (position_source)
                        {
                            auto er = parsePositionSource(position_source);
                            if (er) return er;
                        }
                    }
                    else if (strcmp(vertices_input.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_NORMAL_SEMANTIC) == 0)
                    {
                        std::string normal_source_id = &(vertices_input.attribute(TOKEN_SOURCE).as_string()[1]);
                        pugi::xml_node normal_source = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_SOURCE, normal_source_id);
                        if (normal_source)
                        {
                            auto er = parseNormalSource(normal_source);
                            if (er) return er;
                        }
                    }
                    else if (strcmp(vertices_input.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_TEXCOORD_SEMANTIC) == 0)
                    {
                        std::string texc_source_id = &(vertices_input.attribute(TOKEN_SOURCE).as_string()[1]);
                        pugi::xml_node texcoord_source = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_SOURCE, texc_source_id);
                        if (texcoord_source)
                        {
                            auto er = parseTexcoordSource(texcoord_source, 0);
                            if (er) return er;
                        }
                    }
                    vertices_input = vertices_input.next_sibling(TOKEN_INPUT);
                }
            }
            else
            {
                m_outputPipe("no vertices node!!");
            }
        }
        else if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_NORMAL_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            m_geometryValueOffsets.nor_offset = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node normal_source = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_SOURCE, source_id);
            if (normal_source)
            {
                auto er = parseNormalSource(normal_source);
                if (er) return er;
            }
        }
        else if (strcmp(input_source.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_TEXCOORD_SEMANTIC) == 0)
        {
            std::string source_id = &(input_source.attribute(TOKEN_SOURCE).as_string()[1]);
            int texcoord_set = input_source.attribute(TOKEN_SET).as_int();
            m_geometryValueOffsets.tex_offset[texcoord_set] = input_source.attribute(TOKEN_OFFSET).as_int();
            pugi::xml_node texcoord_source = DaeSchema::findChildNodeWithId(vertices_xml_node, TOKEN_SOURCE, source_id);
            if (texcoord_source)
            {
                auto er = parseTexcoordSource(texcoord_source, texcoord_set);
                if (er) return er;
            }
        }
        input_source = input_source.next_sibling();
    }
    pugi::xml_node index_ary_node = triangles_node.child(TOKEN_INDEX_ARRAY);
    if (index_ary_node)
    {
        auto er = parseIndexArray(index_ary_node, triangle_count);
        if (er) return er;
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::organizeVertices(const std::shared_ptr<DaeVertexWeightsParser>& weights_parser)
{
    bool is_split_vertex = true;
    if (m_geometryValueOffsets.nor_offset >= 0)
    {
        is_split_vertex = false;
    }
    for (int i = 0; i < 4; i++)
    {
        if (m_geometryValueOffsets.tex_offset[i] >= 0) is_split_vertex = false;
    }
    std::error_code er = splitVertexPositions(m_geometryValueOffsets.pos_offset, 0, m_geometryValueOffsets.tex_offset[0], is_split_vertex, weights_parser);
    if (er) return er;
    er = collapseVertexNormals(m_geometryValueOffsets.pos_offset, m_geometryValueOffsets.nor_offset, is_split_vertex);
    if (er) return er;
    er = collapseIndexArray(m_geometryValueOffsets.pos_offset, is_split_vertex);
    return er;
}

void DaeMeshVerticesParser::persistSingleGeometry(const Enigma::Geometries::GeometryId& geo_id, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser, const std::shared_ptr<Enigma::Geometries::GeometryDataStoreMapper>& geometry_store)
{
    Enigma::Geometries::TriangleListAssembler geo_assembler(geo_id);
    geo_assembler.position3s(m_splitedPositions);
    geo_assembler.normals(m_collapsedNormals);
    geo_assembler.indices(m_collapsedIndices);
    geo_assembler.vertexCapacity(static_cast<unsigned>(m_splitedPositions.size()));
    geo_assembler.indexCapacity(static_cast<unsigned>(m_collapsedIndices.size()));
    geo_assembler.vertexUsedCount(static_cast<unsigned>(m_splitedPositions.size()));
    geo_assembler.indexUsedCount(static_cast<unsigned>(m_collapsedIndices.size()));
    geo_assembler.addSegment({ 0, static_cast<unsigned>(m_splitedPositions.size()), 0, static_cast<unsigned>(m_collapsedIndices.size()) });
    geo_assembler.topology(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    if (weights_parser)
    {
        geo_assembler.paletteIndices(weights_parser->getSplitedWeightIndices());
        geo_assembler.weights(weights_parser->getSplitedVertexWeights(), 4);
    }
    geo_assembler.addTexture2DCoords(m_splitedTexCoord[0]);
    geo_assembler.computeAlignedBox();
    geo_assembler.asAsset(geo_id.name(), geo_id.name() + ".geo", "APK_PATH");
    if (geometry_store)
    {
        geometry_store->putGeometry(geo_id, geo_assembler.assemble());
    }
}

std::error_code DaeMeshVerticesParser::parsePositionSource(const pugi::xml_node& position_source)
{
    m_outputPipe("    Parse position source " + std::string(position_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = position_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        m_outputPipe("no float array in position!!");
        return ParserError::noPositionArray;
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_positions.resize(data_count / 3);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 3; i++)
    {
        // y,z 互換不做了
        //ss >> m_positions[i].X() >> m_positions[i].Z() >> m_positions[i].Y();
        ss >> m_positions[i].x() >> m_positions[i].y() >> m_positions[i].z();
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::parseNormalSource(const pugi::xml_node& normal_source)
{
    m_outputPipe("    Parse normal source " + std::string(normal_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = normal_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        m_outputPipe("no float array in normal!!");
        return ParserError::noNormalArray;
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_normals.resize(data_count / 3);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 3; i++)
    {
        // y,z 互換不做了
        //ss >> m_normals[i].X() >> m_normals[i].Z() >> m_normals[i].Y();
        ss >> m_normals[i].x() >> m_normals[i].y() >> m_normals[i].z();
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::parseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set)
{
    m_outputPipe("    Parse tex coord source " + std::string(texcoord_source.attribute(TOKEN_ID).as_string()));
    pugi::xml_node data_ary_node = texcoord_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary_node)
    {
        m_outputPipe("no float array in texcoord!!");
        return ParserError::noTexcoordArray;
    }
    int data_count = data_ary_node.attribute(TOKEN_COUNT).as_int();
    m_texcoords[texcoord_set].resize(data_count / 2);
    std::stringstream ss(std::string(data_ary_node.text().as_string()));
    for (int i = 0; i < data_count / 2; i++)
    {
        ss >> m_texcoords[texcoord_set][i].x() >> m_texcoords[texcoord_set][i].y();
        m_texcoords[texcoord_set][i].y() = 1.0f - m_texcoords[texcoord_set][i].y();  // y 反向
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::parseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count)
{
    m_outputPipe("    Parse index array ");
    m_primitiveIndices.resize(triangle_count * 3 * 3);
    std::stringstream ss(std::string(index_ary_node.text().as_string()));
    for (int i = 0; i < triangle_count * 3 * 3; i++)
    {
        ss >> m_primitiveIndices[i];
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::splitVertexPositions(int pos_offset, int tex_set, int tex_offset, bool is_split_vertex, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser)
{
    if (is_split_vertex)
    {
        m_splitedPositions = m_positions;
        return ParserError::ok;
    }
    // 相同位置，但是不同的貼圖軸，要拆成不同的vertex
    m_splitedPositions.clear();
    m_splitedTexCoord[tex_set].clear();
    if (weights_parser)
    {
        weights_parser->clearSplitedVertexWeights();
    }
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
            if (weights_parser)
            {
                weights_parser->pushSplitedVertexIndex(pos_idx);
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
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::collapseVertexNormals(int pos_offset, int nor_offset, bool is_split_vertex)
{
    if (is_split_vertex) // 不需要 collapse
    {
        m_collapsedNormals = m_normals;
        return ParserError::ok;
    }
    if (nor_offset < 0) return ParserError::ok;
    int vertex_count = static_cast<int>(m_splitedPositions.size());
    m_collapsedNormals.resize(vertex_count);
    int prim_set_count = static_cast<int>(m_primitiveIndices.size()) / 3;
    for (int i = 0; i < vertex_count; i++)
    {
        Enigma::MathLib::Vector3 nor = Enigma::MathLib::Vector3::ZERO;
        std::list<Enigma::MathLib::Vector3> used_normal;  // 要去除掉共平面的normal計算
        for (int p = 0; p < prim_set_count; p++)
        {
            if (m_primitiveIndices[p * 3 + pos_offset] == i) // 對應到頂點
            {
                Enigma::MathLib::Vector3 current_nor = m_normals[m_primitiveIndices[p * 3 + nor_offset]];
                if (std::find_if(used_normal.begin(), used_normal.end(),
                    [=](Enigma::MathLib::Vector3 vec) -> bool { return vec == current_nor; }) == used_normal.end())
                {
                    nor += current_nor;
                    used_normal.push_back(current_nor);
                }
            }
        }
        m_collapsedNormals[i] = nor.normalize();
    }
    return ParserError::ok;
}

std::error_code DaeMeshVerticesParser::collapseIndexArray(int pos_offset, bool is_split_vertex)
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
        return ParserError::ok;
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
    return ParserError::ok;
}
