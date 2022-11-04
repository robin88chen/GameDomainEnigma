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
#include <memory>
#include <any>

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

    protected:
        error GetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int destDimension, float* dest, bool isPos);
        error SetVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int srcDimension, float* src, bool isPos);
        error GetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int destDimension, float* dest, unsigned int count, bool isPos, std::function<bool()> skip_check = nullptr);
        error SetVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int srcDimension, float* src, unsigned int count, bool isPos, std::function<bool()> skip_check = nullptr);

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
