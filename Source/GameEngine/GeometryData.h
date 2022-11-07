/*********************************************************************
 * \file   GeometryData.h
 * \brief  Geometry Data base class, an entity,
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef GEOMETRY_DATA_H
#define GEOMETRY_DATA_H

#include "Frameworks/Rtti.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GeometrySegment.h"
#include "GraphicKernel/IVertexDeclaration.h"
#include "BoundingVolume.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector2.h"
#include <memory>
#include <any>

#include "GraphicKernel/IVertexBuffer.h"

namespace Enigma::Engine
{
    using error = std::error_code;

    class GeometryData : public std::enable_shared_from_this<GeometryData>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        GeometryData(const std::string& name);
        GeometryData(const GeometryData&) = delete;
        GeometryData(GeometryData&&) = delete;
        virtual ~GeometryData();

        GeometryData& operator=(const GeometryData&) = delete;
        GeometryData& operator=(GeometryData&&) = delete;

        const std::string& GetName() { return m_name; }

        /** Create geometry vertex
        @param vertex_format_string vertex format string
        @param vtx_capa vertex data capacity
        @param vtx_count vertex data used count
        @param idx_capa index data capacity
        @param idx_count index data used count */
        virtual error CreateVertexCapacity(const std::string& vertex_format_string,
            unsigned int vtx_capa, unsigned int vtx_count, unsigned int idx_capa = 0, unsigned int idx_count = 0);
        /** resize vertex memory */
        virtual error ReSizeVertexMemoryCapacity(unsigned int vtx_capa, unsigned int idx_capa);
        /** set using vertex count */
        virtual void SetUsingVertexCount(unsigned int vtx_count, unsigned int idx_count = 0);

        /** get position */
        virtual MathLib::Vector3 GetPosition3(unsigned int vtxIndex);
        /** get position */
        virtual MathLib::Vector4 GetPosition4(unsigned int vtxIndex);
        /** get position array */
        virtual std::vector<MathLib::Vector3> GetPosition3Array(unsigned int count);
        virtual std::vector<MathLib::Vector3> GetPosition3Array(unsigned int offset, unsigned int count);
        /** set position */
        virtual error SetPosition3(unsigned int vtxIndex, const MathLib::Vector3& position);
        /** set position */
        virtual error SetPosition4(unsigned int vtxIndex, const MathLib::Vector4& position);
        /** set position array */
        virtual error SetPosition3Array(const std::vector<MathLib::Vector3>& positions);
        virtual error SetPosition3Array(unsigned int offset, const std::vector<MathLib::Vector3>& positions);
        /** set position array */
        virtual error SetPosition4Array(const std::vector<MathLib::Vector4>& positions);
        virtual error SetPosition4Array(unsigned int offset, const std::vector<MathLib::Vector4>& positions);

        /** get vertex normal */
        virtual MathLib::Vector3 GetVertexNormal(unsigned int vtxIndex);
        /** set vertex normal */
        virtual error SetVertexNormal(unsigned int vtxIndex, const MathLib::Vector3& nor);
        /** get vertex normal array */
        virtual std::vector<MathLib::Vector3> GetVertexNormalArray(unsigned int count);
        virtual std::vector<MathLib::Vector3> GetVertexNormalArray(unsigned int offset, unsigned int count);
        /** set vertex normal array */
        virtual error SetVertexNormalArray(const std::vector<MathLib::Vector3>& normals);
        virtual error SetVertexNormalArray(unsigned int offset, const std::vector<MathLib::Vector3>& normals);

        /** set diffuse color */
        virtual error SetDiffuseColorArray(const std::vector<MathLib::Vector4>& color);
        virtual error SetDiffuseColorArray(unsigned int offset, const std::vector<MathLib::Vector4>& color);
        /** set diffuse color */
        virtual error SetDiffuseColorArray(const std::vector<MathLib::ColorRGBA>& color);
        virtual error SetDiffuseColorArray(unsigned int offset, const std::vector<MathLib::ColorRGBA>& color);

        /** set texture coord array */
        virtual error SetTexture2DCoordArray(unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        virtual error SetTexture2DCoordArray(unsigned int offset, unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        /** get texture coord array */
        virtual std::vector<MathLib::Vector2> GetTexture2DCoordArray(unsigned int stage, unsigned int count);
        virtual std::vector<MathLib::Vector2> GetTexture2DCoordArray(unsigned int offset, unsigned int stage, unsigned int count);
        /** set texture coord array */
        virtual error SetTexture1DCoordArray(unsigned int stage, const std::vector<float>& us);
        /** set texture coord array @param count : count of data in array ( vtx count * 3 ) */
        virtual error SetTexture3DCoordArray(unsigned int stage, const std::vector<MathLib::Vector3>& uvws);

        /** set palette index array */
        virtual error SetPaletteIndexArray(const std::vector<unsigned int>& palette_array);
        /** set skin weight array */
        virtual error SetSkinWeightArray(unsigned int weight_idx, const std::vector<float>& weight_array);
        /** set total skin weight array, array size : vtx count * blend weight count */
        virtual error SetTotalSkinWeightArray(const std::vector<float>& weight_array);
        /** set vertex tangent array */
        virtual error SetVertexTangentArray(const std::vector<MathLib::Vector4>& tangent);

        /** set index array */
        virtual error SetIndexArray(const std::vector<unsigned int>& idx_ary);

        /** get primitive type */
        virtual Graphics::PrimitiveTopology GetPrimitiveTopology() { return m_topology; };

        /** get vertex format string */
        virtual std::string GetVertexFormatString() { return m_vertexFormatCode.ToString(); };

        /** get vertex memory */
        virtual const byte_buffer& GetVertexMemory() { return m_vertexMemory; };
        /** get ranged vertex memory */
        virtual Graphics::IVertexBuffer::ranged_buffer GetRangedVertexMemory(unsigned int offset, unsigned int count);
        /** get index memory */
        virtual const uint_buffer& GetIndexMemory() { return m_indexMemory; };

        /** get vertex capacity */
        virtual unsigned int GetVertexCapacity() { return m_vtxCapacity; };
        /** get index capacity */
        virtual unsigned int GetIndexCapacity() { return m_idxCapacity; };

        /** get used vertex count */
        virtual unsigned int GetUsedVertexCount() { return m_vtxUsedCount; };
        /** get used index count */
        virtual unsigned int GetUsedIndexCount() { return m_idxUsedCount; };

        /** size of vertex (in byte) */
        virtual unsigned int SizeofVertex() { return m_vertexDesc.m_totalVertexSize; };

        /** get geometry segment */
        virtual const GeometrySegment& GetSegment(unsigned int index) const;
        /** get geometry segment count */
        virtual unsigned int GetSegmentCount() { return static_cast<unsigned int>(m_geoSegmentVector.size()); };
        /** get geometry segment vector */
        virtual const GeometrySegmentVector& GetSegmentVector() const { return m_geoSegmentVector; };
        /** change segment */
        virtual void ChangeSegment(unsigned int index, unsigned int start_vtx, unsigned int vtx_count, unsigned int start_idx, unsigned int idx_count);
        /** resize segment array */
        virtual void ResizeSegmentVector(unsigned int new_size) { m_geoSegmentVector.resize(new_size, GeometrySegment()); };

        /** calculate bounding volume */
        virtual void CalculateBoundingVolume(bool axis_align);
        /** get bounding volume */
        const BoundingVolume& GetBoundingVolume() { return m_geometryBound; };

    protected:
        error GetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int destDimension, float* dest, bool isPos);
        error SetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int srcDimension, const float* src, bool isPos);
        error GetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int destDimension, float* dest, unsigned int count, bool isPos, std::function<bool()> skip_check = nullptr);
        error SetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int srcDimension, const float* src, unsigned int count, bool isPos, std::function<bool()> skip_check = nullptr);

    protected:
        std::string m_name;

        GeometrySegmentVector m_geoSegmentVector;

        Graphics::IVertexDeclaration::VertexFormatCode m_vertexFormatCode;
        Graphics::IVertexDeclaration::VertexDesc m_vertexDesc;

        byte_buffer m_vertexMemory;
        uint_buffer m_indexMemory;

        unsigned int m_vtxCapacity;
        unsigned int m_idxCapacity;
        unsigned int m_vtxUsedCount;
        unsigned int m_idxUsedCount;

        Graphics::PrimitiveTopology m_topology;

        BoundingVolume m_geometryBound;
    };

    using GeometryDataPtr = std::shared_ptr<GeometryData>;
}

#endif // GEOMETRY_DATA_H
