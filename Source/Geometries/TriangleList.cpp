﻿#include "TriangleList.h"
#include "Platforms/PlatformLayer.h"
#include "GraphicKernel/VertexDescription.h"
#include "GeometryErrors.h"
#include "MathLib/MathAlgorithm.h"
#include "TriangleListAssembler.h"

using namespace Enigma::Geometries;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Graphics;

DEFINE_RTTI(Geometries, TriangleList, GeometryData);

TriangleList::TriangleList(const GeometryId& id) : GeometryData(id)
{
    m_factoryDesc = FactoryDesc(TriangleList::TYPE_RTTI.getName());
    m_topology = Graphics::PrimitiveTopology::Topology_TriangleList;
}

TriangleList::~TriangleList()
{
}

std::shared_ptr<TriangleList> TriangleList::create(const GeometryId& id)
{
    return std::make_shared<TriangleList>(id);
}

std::shared_ptr<GeometryAssembler> TriangleList::assembler() const
{
    return std::make_shared<TriangleListAssembler>(m_id);
}

std::shared_ptr<GeometryDisassembler> TriangleList::disassembler()
{
    return std::make_shared<TriangleListDisassembler>();
}

unsigned TriangleList::getTriangleCount()
{
    if (m_idxUsedCount) return m_idxUsedCount / 3;
    return m_vtxUsedCount / 3;
}

void TriangleList::fetchTriangleVertexIndex(unsigned idx, unsigned vtx_idx[3])
{
    if ((m_idxUsedCount) && (!m_indexMemory.empty()))
    {
        vtx_idx[0] = m_indexMemory[idx * 3];
        vtx_idx[1] = m_indexMemory[idx * 3 + 1];
        vtx_idx[2] = m_indexMemory[idx * 3 + 2];
    }
    else
    {
        vtx_idx[0] = idx * 3;
        vtx_idx[1] = idx * 3 + 1;
        vtx_idx[2] = idx * 3 + 2;
    }
}

void TriangleList::fetchTrianglePos(unsigned idx, MathLib::Vector3 tri[3])
{
    unsigned int vtx_idx[3];
    fetchTriangleVertexIndex(idx, vtx_idx);
    unsigned int vtx_pitch = sizeofVertex();
    memcpy(&tri[0], &m_vertexMemory[vtx_idx[0] * vtx_pitch], sizeof(Vector3));
    memcpy(&tri[1], &m_vertexMemory[vtx_idx[1] * vtx_pitch], sizeof(Vector3));
    memcpy(&tri[2], &m_vertexMemory[vtx_idx[2] * vtx_pitch], sizeof(Vector3));
}

void TriangleList::fetchTriangleTextureCoord(unsigned idx, unsigned tex_channel, MathLib::Vector2 uv[3])
{
    if (FATAL_LOG_EXPR(tex_channel >= VertexFormatCode::MAX_TEX_COORD)) return;
    if (m_vertexDesc.textureCoordOffset(tex_channel) < 0) return;
    if (m_vertexDesc.textureCoordSize(tex_channel) < 2) return;

    unsigned int vtx_idx[3];
    fetchTriangleVertexIndex(idx, vtx_idx);

    unsigned int vtx_pitch = sizeofVertex();
    unsigned int offset = m_vertexDesc.textureCoordOffset(tex_channel) * sizeof(float);
    memcpy(&uv[0], &m_vertexMemory[vtx_idx[0] * vtx_pitch + offset], sizeof(Vector2));
    memcpy(&uv[1], &m_vertexMemory[vtx_idx[1] * vtx_pitch + offset], sizeof(Vector2));
    memcpy(&uv[2], &m_vertexMemory[vtx_idx[2] * vtx_pitch + offset], sizeof(Vector2));
}

error TriangleList::calculateVertexTangentSpace(unsigned tex_channel)
{
    if (FATAL_LOG_EXPR(tex_channel >= VertexFormatCode::MAX_TEX_COORD))
        return ErrorCode::invalidArrayIndex;

    if (FATAL_LOG_EXPR(m_vertexMemory.empty())) return ErrorCode::nullMemoryBuffer;
    if (!m_vertexDesc.hasTangent()) return ErrorCode::ok;
    if (m_vertexDesc.textureCoordOffset(tex_channel) < 0) return ErrorCode::ok;

    std::vector<Vector3> tangent_buf;
    tangent_buf.resize(m_vtxUsedCount);
    std::vector<Vector3> binormal_buf;
    binormal_buf.resize(m_vtxUsedCount);

    Vector3 pos[3];
    Vector2 uv[3];
    unsigned int vtx_idx[3];
    unsigned int triangle_count = getTriangleCount();
    for (unsigned int ti = 0; ti < triangle_count; ti++)
    {
        fetchTrianglePos(ti, pos);
        fetchTriangleTextureCoord(ti, tex_channel, uv);
        fetchTriangleVertexIndex(ti, vtx_idx);

        auto [tangent, binormal] = MathAlgorithm::CalculateTangentVector(pos, uv);
        tangent_buf[vtx_idx[0]] += tangent;
        tangent_buf[vtx_idx[1]] += tangent;
        tangent_buf[vtx_idx[2]] += tangent;
        binormal_buf[vtx_idx[0]] += binormal;
        binormal_buf[vtx_idx[1]] += binormal;
        binormal_buf[vtx_idx[2]] += binormal;
    }

    std::vector<Vector4> vtxTangent;
    vtxTangent.resize(m_vtxUsedCount);
    // normalize and set to geometry
    for (unsigned int vi = 0; vi < m_vtxUsedCount; vi++)
    {
        Vector3 nor = getVertexNormal(vi);
        Vector3 tangent = tangent_buf[vi] - nor * nor.dot(tangent_buf[vi]);
        tangent.normalizeSelf();
        float binor_w = 1.0f;
        Vector3 nxt = nor.cross(tangent_buf[vi]);
        if (nxt.dot(binormal_buf[vi]) < 0.0f)
        {
            binor_w = -1.0f;
        }
        vtxTangent[vi] = Vector4(tangent.x(), tangent.y(), tangent.z(), binor_w);
    }

    setVertexTangentArray(vtxTangent);

    return ErrorCode::ok;
}
