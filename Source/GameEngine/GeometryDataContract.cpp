#include "GeometryDataContract.h"
#include "GraphicKernel/VertexDescription.h"
#include "GeometryData.h"
#include "TriangleList.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Graphics;

std::string TOKEN_NAME = "Name";
std::string TOKEN_VERTEX_FORMAT = "VertexFormat";
std::string TOKEN_SEGMENTS = "Segments";
std::string TOKEN_POSITIONS_3 = "Positions3";
std::string TOKEN_POSITIONS_4 = "Positions4";
std::string TOKEN_NORMALS = "Normals";
std::string TOKEN_DIFFUSE_COLORS = "DiffuseColors";
std::string TOKEN_SPECULAR_COLORS = "SpecularColors";
std::string TOKEN_TEX_COORD0 = "TexCoord0";
std::string TOKEN_TEX_COORD1 = "TexCoord1";
std::string TOKEN_TEX_COORD2 = "TexCoord2";
std::string TOKEN_TEX_COORD3 = "TexCoord3";
std::string TOKEN_TEX_COORD4 = "TexCoord4";
std::string TOKEN_TEX_COORD5 = "TexCoord5";
std::string TOKEN_TEX_COORD6 = "TexCoord6";
std::string TOKEN_TEX_COORD7 = "TexCoord7";
std::string TOKEN_PALETTE_INDICES = "PaletteIndices";
std::string TOKEN_WEIGHTS = "Weights";
std::string TOKEN_TANGENTS = "Tangents";
std::string TOKEN_INDICES = "Indices";
std::string TOKEN_VERTEX_CAPACITY = "VertexCapacity";
std::string TOKEN_INDEX_CAPACITY = "IndexCapacity";
std::string TOKEN_VERTEX_USED_COUNT = "VertexUsedCount";
std::string TOKEN_INDEX_USED_COUNT = "IndexUsedCount";
std::string TOKEN_TOPOLOGY = "Topology";
std::string TOKEN_GEOMETRY_BOUND = "GeometryBound";
std::string TOKEN_2D_COORDS = "2DCoords";
std::string TOKEN_1D_COORDS = "1DCoords";
std::string TOKEN_3D_COORDS = "3DCoords";
std::array<std::string, VertexFormatCode::MAX_TEX_COORD> TOKEN_TEX_COORDS =
{
    TOKEN_TEX_COORD0, TOKEN_TEX_COORD1, TOKEN_TEX_COORD2, TOKEN_TEX_COORD3,
    TOKEN_TEX_COORD4, TOKEN_TEX_COORD5, TOKEN_TEX_COORD6, TOKEN_TEX_COORD7
};


TextureCoordContract TextureCoordContract::FromContract(const Contract& contract)
{
    TextureCoordContract coords;
    if (contract.HasValue(TOKEN_2D_COORDS))
    {
        coords.m_2dCoords = contract.TryGetValue<std::vector<Vector2>>(TOKEN_2D_COORDS).value();
    }
    else if (contract.HasValue(TOKEN_1D_COORDS))
    {
        coords.m_1dCoords = contract.TryGetValue<std::vector<float>>(TOKEN_1D_COORDS).value();
    }
    else if (contract.HasValue(TOKEN_3D_COORDS))
    {
        coords.m_3dCoords = contract.TryGetValue<std::vector<Vector3>>(TOKEN_3D_COORDS).value();
    }
    return coords;
}

Contract TextureCoordContract::ToContract()
{
    Contract contract;
    if (m_2dCoords)
    {
        contract.AddOrUpdate(TOKEN_2D_COORDS, m_2dCoords.value());
    }
    else if (m_1dCoords)
    {
        contract.AddOrUpdate(TOKEN_1D_COORDS, m_1dCoords.value());
    }
    else if (m_3dCoords)
    {
        contract.AddOrUpdate(TOKEN_3D_COORDS, m_3dCoords.value());
    }
    return contract;
}

//-----------------------------------------------------------------------------------
GeometryDataContract GeometryDataContract::FromContract(const Contract& contract)
{
    GeometryDataContract geometry;
    geometry.m_name = contract.TryGetValue<std::string>(TOKEN_NAME).value();
    geometry.m_vertexFormat = contract.TryGetValue<std::string>(TOKEN_VERTEX_FORMAT).value();
    geometry.m_segments = contract.TryGetValue<std::vector<unsigned>>(TOKEN_SEGMENTS).value();
    if (contract.HasValue(TOKEN_POSITIONS_3))
    {
        geometry.m_position3s = contract.TryGetValue<std::vector<Vector3>>(TOKEN_POSITIONS_3).value();
    }
    if (contract.HasValue(TOKEN_POSITIONS_4))
    {
        geometry.m_position4s = contract.TryGetValue<std::vector<Vector4>>(TOKEN_POSITIONS_4).value();
    }
    if (contract.HasValue(TOKEN_NORMALS))
    {
        geometry.m_normals = contract.TryGetValue<std::vector<Vector3>>(TOKEN_NORMALS).value();
    }
    if (contract.HasValue(TOKEN_DIFFUSE_COLORS))
    {
        geometry.m_diffuseColors = contract.TryGetValue<std::vector<Vector4>>(TOKEN_DIFFUSE_COLORS).value();
    }
    if (contract.HasValue(TOKEN_SPECULAR_COLORS))
    {
        geometry.m_specularColors = contract.TryGetValue<std::vector<Vector4>>(TOKEN_SPECULAR_COLORS).value();
    }
    for (auto& token_tex_coord : TOKEN_TEX_COORDS)
    {
        if (contract.HasValue(token_tex_coord))
        {
            geometry.m_texCoords.emplace_back(contract.TryGetValue<Contract>(token_tex_coord).value());
        }
    }
    if (contract.HasValue(TOKEN_PALETTE_INDICES))
    {
        geometry.m_paletteIndices = contract.TryGetValue<std::vector<unsigned>>(TOKEN_PALETTE_INDICES).value();
    }
    if (contract.HasValue(TOKEN_WEIGHTS))
    {
        geometry.m_weights = contract.TryGetValue<std::vector<float>>(TOKEN_WEIGHTS).value();
    }
    if (contract.HasValue(TOKEN_TANGENTS))
    {
        geometry.m_tangents = contract.TryGetValue<std::vector<Vector4>>(TOKEN_TANGENTS).value();
    }
    if (contract.HasValue(TOKEN_INDICES))
    {
        geometry.m_indices = contract.TryGetValue<std::vector<unsigned>>(TOKEN_INDICES).value();
    }
    geometry.m_vtxCapacity = contract.TryGetValue<unsigned>(TOKEN_VERTEX_CAPACITY).value();
    geometry.m_idxCapacity = contract.TryGetValue<unsigned>(TOKEN_INDEX_CAPACITY).value();
    geometry.m_vtxUsedCount = contract.TryGetValue<unsigned>(TOKEN_VERTEX_USED_COUNT).value();
    geometry.m_idxUsedCount = contract.TryGetValue<unsigned>(TOKEN_INDEX_USED_COUNT).value();
    geometry.m_topology = contract.TryGetValue<unsigned>(TOKEN_TOPOLOGY).value();
    geometry.m_geometryBound = contract.TryGetValue<Contract>(TOKEN_GEOMETRY_BOUND).value();

    return geometry;
}

Contract GeometryDataContract::ToContract()
{
    Contract contract;
    contract.AddRtti(FactoryDesc(GeometryData::TYPE_RTTI.GetName()));
    contract.AddOrUpdate(TOKEN_NAME, m_name);
    contract.AddOrUpdate(TOKEN_VERTEX_FORMAT, m_vertexFormat);
    contract.AddOrUpdate(TOKEN_SEGMENTS, m_segments);
    if (m_position3s)
    {
        contract.AddOrUpdate(TOKEN_POSITIONS_3, m_position3s.value());
    }
    if (m_position4s)
    {
        contract.AddOrUpdate(TOKEN_POSITIONS_4, m_position4s.value());
    }
    if (m_normals)
    {
        contract.AddOrUpdate(TOKEN_NORMALS, m_normals.value());
    }
    if (m_diffuseColors)
    {
        contract.AddOrUpdate(TOKEN_DIFFUSE_COLORS, m_diffuseColors.value());
    }
    if (m_specularColors)
    {
        contract.AddOrUpdate(TOKEN_SPECULAR_COLORS, m_specularColors.value());
    }
    for (unsigned i = 0; i < m_texCoords.size(); i++)
    {
        contract.AddOrUpdate(TOKEN_TEX_COORDS[i], m_texCoords[i]);
    }
    if (m_paletteIndices)
    {
        contract.AddOrUpdate(TOKEN_PALETTE_INDICES, m_paletteIndices.value());
    }
    if (m_weights)
    {
        contract.AddOrUpdate(TOKEN_WEIGHTS, m_weights.value());
    }
    if (m_tangents)
    {
        contract.AddOrUpdate(TOKEN_TANGENTS, m_tangents.value());
    }
    if (m_indices)
    {
        contract.AddOrUpdate(TOKEN_INDICES, m_indices.value());
    }
    contract.AddOrUpdate(TOKEN_VERTEX_CAPACITY, m_vtxCapacity);
    contract.AddOrUpdate(TOKEN_INDEX_CAPACITY, m_idxCapacity);
    contract.AddOrUpdate(TOKEN_VERTEX_USED_COUNT, m_vtxUsedCount);
    contract.AddOrUpdate(TOKEN_INDEX_USED_COUNT, m_idxUsedCount);
    contract.AddOrUpdate(TOKEN_TOPOLOGY, m_topology);
    contract.AddOrUpdate(TOKEN_GEOMETRY_BOUND, m_geometryBound);

    return contract;
}

TriangleListContract::TriangleListContract(const GeometryDataContract& geometry_contract) : GeometryDataContract(geometry_contract)
{
}

TriangleListContract TriangleListContract::FromContract(const Engine::Contract& contract)
{
    return TriangleListContract(GeometryDataContract::FromContract(contract));
}

Contract TriangleListContract::ToContract()
{
    Contract contract = GeometryDataContract::ToContract();
    contract.AddRtti(FactoryDesc(TriangleList::TYPE_RTTI.GetName()));

    return contract;
}
