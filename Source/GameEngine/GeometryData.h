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
#include "GraphicKernel/VertexDescription.h"
#include "BoundingVolume.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector2.h"
#include "GraphicKernel/IVertexBuffer.h"
#include "GraphicKernel/IIndexBuffer.h"
#include "GeometryDataDto.h"
#include "RenderBufferSignature.h"
#include <memory>


namespace Enigma::Engine
{
    using error = std::error_code;

    class GeometryData : public std::enable_shared_from_this<GeometryData>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        GeometryData(const std::string& name);
        GeometryData(const GenericDto& dto);
        GeometryData(const GeometryData&) = delete;
        GeometryData(GeometryData&&) = delete;
        virtual ~GeometryData();

        GeometryData& operator=(const GeometryData&) = delete;
        GeometryData& operator=(GeometryData&&) = delete;

        virtual GenericDto SerializeDto();

        const std::string& GetName() { return m_name; }

        const FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }

        RenderBufferSignature MakeRenderBufferSignature() const;

        /** Create geometry vertex
        @param vertex_format_string vertex format string
        @param vtx_capa vertex data capacity
        @param vtx_count vertex data used count
        @param idx_capa index data capacity
        @param idx_count index data used count */
        error CreateVertexCapacity(const std::string& vertex_format_string,
            unsigned int vtx_capa, unsigned int vtx_count, unsigned int idx_capa = 0, unsigned int idx_count = 0);
        /** resize vertex memory */
        error ReSizeVertexMemoryCapacity(unsigned int vtx_capa, unsigned int idx_capa);
        /** set using vertex count */
        void SetUsingVertexCount(unsigned int vtx_count, unsigned int idx_count = 0);

        /** get position */
        MathLib::Vector3 GetPosition3(unsigned int vtxIndex);
        /** get position */
        MathLib::Vector4 GetPosition4(unsigned int vtxIndex);
        /** get position array */
        std::vector<MathLib::Vector3> GetPosition3Array(unsigned int count);
        std::vector<MathLib::Vector3> GetPosition3Array(unsigned int offset, unsigned int count);
        std::vector<MathLib::Vector4> GetPosition4Array(unsigned int count);
        std::vector<MathLib::Vector4> GetPosition4Array(unsigned int offset, unsigned int count);
        /** set position */
        error SetPosition3(unsigned int vtxIndex, const MathLib::Vector3& position);
        /** set position */
        error SetPosition4(unsigned int vtxIndex, const MathLib::Vector4& position);
        /** set position array */
        error SetPosition3Array(const std::vector<MathLib::Vector3>& positions);
        error SetPosition3Array(unsigned int offset, const std::vector<MathLib::Vector3>& positions);
        /** set position array */
        error SetPosition4Array(const std::vector<MathLib::Vector4>& positions);
        error SetPosition4Array(unsigned int offset, const std::vector<MathLib::Vector4>& positions);

        /** get vertex normal */
        MathLib::Vector3 GetVertexNormal(unsigned int vtxIndex);
        /** set vertex normal */
        error SetVertexNormal(unsigned int vtxIndex, const MathLib::Vector3& nor);
        /** get vertex normal array */
        std::vector<MathLib::Vector3> GetVertexNormalArray(unsigned int count);
        std::vector<MathLib::Vector3> GetVertexNormalArray(unsigned int offset, unsigned int count);
        /** set vertex normal array */
        error SetVertexNormalArray(const std::vector<MathLib::Vector3>& normals);
        error SetVertexNormalArray(unsigned int offset, const std::vector<MathLib::Vector3>& normals);

        /** set diffuse color */
        error SetDiffuseColorArray(const std::vector<MathLib::Vector4>& color);
        error SetDiffuseColorArray(unsigned int offset, const std::vector<MathLib::Vector4>& color);
        /** set diffuse color */
        error SetDiffuseColorArray(const std::vector<MathLib::ColorRGBA>& color);
        error SetDiffuseColorArray(unsigned int offset, const std::vector<MathLib::ColorRGBA>& color);
        /** get diffuse color */
        std::vector<MathLib::Vector4> GetDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int count);
        std::vector<MathLib::Vector4> GetDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int offset, unsigned int count);
        /** set specular color */
        error SetSpecularColorArray(const std::vector<MathLib::Vector4>& color);
        error SetSpecularColorArray(unsigned int offset, const std::vector<MathLib::Vector4>& color);
        /** get specular color */
        std::vector<MathLib::Vector4> GetSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int count);
        std::vector<MathLib::Vector4> GetSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int offset, unsigned int count);

        /** set texture coord array */
        error SetTexture2DCoordArray(unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        error SetTexture2DCoordArray(unsigned int offset, unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        /** get texture coord array */
        std::vector<MathLib::Vector2> GetTexture2DCoordArray(unsigned int stage, unsigned int count);
        std::vector<MathLib::Vector2> GetTexture2DCoordArray(unsigned int offset, unsigned int stage, unsigned int count);
        /** set texture coord array */
        error SetTexture1DCoordArray(unsigned int stage, const std::vector<float>& us);
        /** get texture coord array */
        std::vector<float> GetTexture1DCoordArray(unsigned int stage, unsigned int count);
        std::vector<float> GetTexture1DCoordArray(unsigned int offset, unsigned int stage, unsigned int count);
        /** set texture coord array @param count : count of data in array ( vtx count * 3 ) */
        error SetTexture3DCoordArray(unsigned int stage, const std::vector<MathLib::Vector3>& uvws);
        /** get texture coord array */
        std::vector<MathLib::Vector3> GetTexture3DCoordArray(unsigned int stage, unsigned int count);
        std::vector<MathLib::Vector3> GetTexture3DCoordArray(unsigned int offset, unsigned int stage, unsigned int count);

        /** set palette index array */
        error SetPaletteIndexArray(const std::vector<unsigned int>& palette_array);
        /** get palette index array */
        std::vector<unsigned> GetPaletteIndexArray(unsigned int count);
        std::vector<unsigned> GetPaletteIndexArray(unsigned int offset, unsigned int count);
        /** set skin weight array */
        error SetSkinWeightArray(unsigned int weight_idx, const std::vector<float>& weight_array);
        /** set total skin weight array, array size : vtx count * blend weight count */
        error SetTotalSkinWeightArray(const std::vector<float>& weight_array);
        /** get total skin weight array, array size : vtx count * blend weight count */
        std::vector<float> GetTotalSkinWeightArray(unsigned int vtx_count);
        std::vector<float> GetTotalSkinWeightArray(unsigned int offset, unsigned int vtx_count);
        /** set vertex tangent array */
        error SetVertexTangentArray(const std::vector<MathLib::Vector4>& tangent);
        /** get vertex tangent array */
        std::vector<MathLib::Vector4> GetVertexTangentArray(unsigned int count);
        std::vector<MathLib::Vector4> GetVertexTangentArray(unsigned int offset, unsigned int count);

        /** set index array */
        error SetIndexArray(const std::vector<unsigned int>& idx_ary);

        /** get primitive type */
        Graphics::PrimitiveTopology GetPrimitiveTopology() { return m_topology; };

        /** get vertex format string */
        std::string GetVertexFormatString() { return m_vertexFormatCode.ToString(); };

        /** get vertex memory */
        const byte_buffer& GetVertexMemory() { return m_vertexMemory; };
        /** get ranged vertex memory */
        Graphics::IVertexBuffer::ranged_buffer GetRangedVertexMemory(unsigned int offset, unsigned int count);
        /** get index memory */
        const uint_buffer& GetIndexMemory() { return m_indexMemory; };
        /** get ranged index memory */
        Graphics::IIndexBuffer::ranged_buffer GetRangedIndexMemory(unsigned int offset, unsigned int count);

        /** get vertex capacity */
        unsigned int GetVertexCapacity() { return m_vtxCapacity; };
        /** get index capacity */
        unsigned int GetIndexCapacity() { return m_idxCapacity; };

        /** get used vertex count */
        unsigned int GetUsedVertexCount() { return m_vtxUsedCount; };
        /** get used index count */
        unsigned int GetUsedIndexCount() { return m_idxUsedCount; };

        /** size of vertex (in byte) */
        unsigned int SizeofVertex() { return m_vertexDesc.TotalVertexSize(); };

        /** get geometry segment */
        const GeometrySegment& GetSegment(unsigned int index) const;
        /** get geometry segment count */
        unsigned int GetSegmentCount() { return static_cast<unsigned int>(m_geoSegmentVector.size()); };
        /** get geometry segment vector */
        const GeometrySegmentVector& GetSegmentVector() const { return m_geoSegmentVector; };
        /** change segment */
        void ChangeSegment(unsigned int index, unsigned int start_vtx, unsigned int vtx_count, unsigned int start_idx, unsigned int idx_count);
        /** resize segment array */
        void ResizeSegmentVector(unsigned int new_size) { m_geoSegmentVector.resize(new_size, GeometrySegment()); };

        /** calculate bounding volume */
        void CalculateBoundingVolume(bool axis_align);
        /** get bounding volume */
        const BoundingVolume& GetBoundingVolume() { return m_geometryBound; };

    protected:
        error GetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int destDimension, float* dest, bool isPos);
        error SetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int srcDimension, const float* src, bool isPos);
        error GetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int destDimension, float* dest, unsigned int count, bool isPos);
        error SetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int srcDimension, const float* src, unsigned int count, bool isPos);

        GeometryDataDto SerializeGeometryDto();

    protected:
        FactoryDesc m_factoryDesc;
        std::string m_name;

        GeometrySegmentVector m_geoSegmentVector;

        Graphics::VertexFormatCode m_vertexFormatCode;
        Graphics::VertexDescription m_vertexDesc;

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
