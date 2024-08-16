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
#include "GameEngine/BoundingVolume.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector2.h"
#include "GraphicKernel/IVertexBuffer.h"
#include "GraphicKernel/IIndexBuffer.h"
#include "GeometryDataDto.h"
#include "GameEngine/RenderBufferSignature.h"
#include "GeometryId.h"
#include <memory>

namespace Enigma::Geometries
{
    using error = std::error_code;

    class GeometryAssembler;

    class GeometryData : public std::enable_shared_from_this<GeometryData>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        GeometryData(const GeometryId& id);
        GeometryData(const GeometryId& id, const Engine::GenericDto& dto);
        GeometryData(const GeometryData&) = delete;
        GeometryData(GeometryData&&) = delete;
        virtual ~GeometryData();

        GeometryData& operator=(const GeometryData&) = delete;
        GeometryData& operator=(GeometryData&&) = delete;

        static std::shared_ptr<GeometryData> queryGeometryData(const GeometryId& id);

        virtual std::shared_ptr<GeometryAssembler> assembler() const = 0;
        virtual void assemble(const std::shared_ptr<GeometryAssembler>& assembler) const;

        const GeometryId& id() { return m_id; }

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        Engine::RenderBufferSignature makeRenderBufferSignature() const;

        /** Create geometry vertex
        @param vertex_format_string vertex format string
        @param vtx_capa vertex data capacity
        @param vtx_count vertex data used count
        @param idx_capa index data capacity
        @param idx_count index data used count */
        error createVertexCapacity(const std::string& vertex_format_string,
            unsigned int vtx_capa, unsigned int vtx_count, unsigned int idx_capa = 0, unsigned int idx_count = 0);
        /** resize vertex memory */
        error resizeVertexMemoryCapacity(unsigned int vtx_capa, unsigned int idx_capa);
        /** set using vertex count */
        void setUsingVertexCount(unsigned int vtx_count, unsigned int idx_count = 0);

        /** get position */
        MathLib::Vector3 getPosition3(unsigned int vtxIndex);
        /** get position */
        MathLib::Vector4 getPosition4(unsigned int vtxIndex);
        /** get position array */
        std::vector<MathLib::Vector3> getPosition3Array(unsigned int count) const;
        std::vector<MathLib::Vector3> getPosition3Array(unsigned int offset, unsigned int count) const;
        std::vector<MathLib::Vector4> getPosition4Array(unsigned int count) const;
        std::vector<MathLib::Vector4> getPosition4Array(unsigned int offset, unsigned int count) const;
        /** set position */
        error setPosition3(unsigned int vtxIndex, const MathLib::Vector3& position);
        /** set position */
        error setPosition4(unsigned int vtxIndex, const MathLib::Vector4& position);
        /** set position array */
        error setPosition3Array(const std::vector<MathLib::Vector3>& positions);
        error setPosition3Array(unsigned int offset, const std::vector<MathLib::Vector3>& positions);
        /** set position array */
        error setPosition4Array(const std::vector<MathLib::Vector4>& positions);
        error setPosition4Array(unsigned int offset, const std::vector<MathLib::Vector4>& positions);

        /** get vertex normal */
        MathLib::Vector3 getVertexNormal(unsigned int vtxIndex) const;
        /** set vertex normal */
        error setVertexNormal(unsigned int vtxIndex, const MathLib::Vector3& nor);
        /** get vertex normal array */
        std::vector<MathLib::Vector3> getVertexNormalArray(unsigned int count) const;
        std::vector<MathLib::Vector3> getVertexNormalArray(unsigned int offset, unsigned int count) const;
        /** set vertex normal array */
        error setVertexNormalArray(const std::vector<MathLib::Vector3>& normals);
        error setVertexNormalArray(unsigned int offset, const std::vector<MathLib::Vector3>& normals);

        /** set diffuse color */
        error setDiffuseColorArray(const std::vector<MathLib::Vector4>& color);
        error setDiffuseColorArray(unsigned int offset, const std::vector<MathLib::Vector4>& color);
        /** set diffuse color */
        error setDiffuseColorArray(const std::vector<MathLib::ColorRGBA>& color);
        error setDiffuseColorArray(unsigned int offset, const std::vector<MathLib::ColorRGBA>& color);
        /** get diffuse color */
        std::vector<MathLib::Vector4> getDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int count) const;
        std::vector<MathLib::Vector4> getDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int offset, unsigned int count) const;
        /** set specular color */
        error setSpecularColorArray(const std::vector<MathLib::Vector4>& color);
        error setSpecularColorArray(unsigned int offset, const std::vector<MathLib::Vector4>& color);
        /** get specular color */
        std::vector<MathLib::Vector4> getSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int count) const;
        std::vector<MathLib::Vector4> getSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned int offset, unsigned int count) const;

        /** set texture coord array */
        error setTexture2DCoordArray(unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        error setTexture2DCoordArray(unsigned int offset, unsigned int stage, const std::vector<MathLib::Vector2>& uvs);
        /** get texture coord array */
        std::vector<MathLib::Vector2> getTexture2DCoordArray(unsigned int stage, unsigned int count) const;
        std::vector<MathLib::Vector2> getTexture2DCoordArray(unsigned int offset, unsigned int stage, unsigned int count) const;
        /** set texture coord array */
        error setTexture1DCoordArray(unsigned int stage, const std::vector<float>& us);
        /** get texture coord array */
        std::vector<float> getTexture1DCoordArray(unsigned int stage, unsigned int count) const;
        std::vector<float> getTexture1DCoordArray(unsigned int offset, unsigned int stage, unsigned int count) const;
        /** set texture coord array */
        error setTexture3DCoordArray(unsigned int stage, const std::vector<MathLib::Vector3>& uvws);
        /** get texture coord array */
        std::vector<MathLib::Vector3> getTexture3DCoordArray(unsigned int stage, unsigned int count) const;
        std::vector<MathLib::Vector3> getTexture3DCoordArray(unsigned int offset, unsigned int stage, unsigned int count) const;

        /** set palette index array */
        error setPaletteIndexArray(const std::vector<unsigned int>& palette_array);
        /** get palette index array */
        std::vector<unsigned> getPaletteIndexArray(unsigned int count) const;
        std::vector<unsigned> getPaletteIndexArray(unsigned int offset, unsigned int count) const;
        /** set skin weight array */
        error setSkinWeightArray(unsigned int weight_idx, const std::vector<float>& weight_array);
        /** set total skin weight array, array size : vtx count * blend weight count */
        error setTotalSkinWeightArray(const std::vector<float>& weight_array);
        /** get total skin weight array, array size : vtx count * blend weight count */
        std::vector<float> getTotalSkinWeightArray(unsigned int vtx_count) const;
        std::vector<float> getTotalSkinWeightArray(unsigned int offset, unsigned int vtx_count) const;
        /** set vertex tangent array */
        error setVertexTangentArray(const std::vector<MathLib::Vector4>& tangent);
        /** get vertex tangent array */
        std::vector<MathLib::Vector4> getVertexTangentArray(unsigned int count) const;
        std::vector<MathLib::Vector4> getVertexTangentArray(unsigned int offset, unsigned int count) const;

        /** set index array */
        error setIndexArray(const std::vector<unsigned int>& idx_ary);

        /** get primitive type */
        Graphics::PrimitiveTopology getPrimitiveTopology() const { return m_topology; };

        /** get vertex format string */
        std::string getVertexFormatString() const { return m_vertexFormatCode.toString(); };

        /** get vertex memory */
        const byte_buffer& getVertexMemory() const { return m_vertexMemory; };
        /** get ranged vertex memory */
        Graphics::IVertexBuffer::ranged_buffer getRangedVertexMemory(unsigned int offset, unsigned int count) const;
        /** get index memory */
        const uint_buffer& getIndexMemory() const { return m_indexMemory; };
        /** get ranged index memory */
        Graphics::IIndexBuffer::ranged_buffer getRangedIndexMemory(unsigned int offset, unsigned int count) const;

        /** get vertex capacity */
        unsigned int getVertexCapacity() const { return m_vtxCapacity; };
        /** get index capacity */
        unsigned int getIndexCapacity() const { return m_idxCapacity; };

        /** get used vertex count */
        unsigned int getUsedVertexCount() const { return m_vtxUsedCount; };
        /** get used index count */
        unsigned int getUsedIndexCount() const { return m_idxUsedCount; };

        /** size of vertex (in byte) */
        unsigned int sizeofVertex() const { return m_vertexDesc.totalVertexSize(); };

        /** get geometry segment */
        const GeometrySegment& getSegment(unsigned int index) const;
        /** get geometry segment count */
        unsigned int getSegmentCount() const { return static_cast<unsigned int>(m_geoSegmentVector.size()); };
        /** get geometry segment vector */
        const GeometrySegmentVector& getSegmentVector() const { return m_geoSegmentVector; };
        /** change segment */
        void changeSegment(unsigned int index, unsigned int start_vtx, unsigned int vtx_count, unsigned int start_idx, unsigned int idx_count);
        /** resize segment array */
        void resizeSegmentVector(unsigned int new_size) { m_geoSegmentVector.resize(new_size, GeometrySegment()); };

        /** calculate bounding volume */
        void calculateBoundingVolume(bool axis_align);
        /** get bounding volume */
        const Engine::BoundingVolume& getBoundingVolume() const { return m_geometryBound; };

    protected:
        error getVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int destDimension, float* dest, bool isPos) const;
        error setVertexMemoryData(unsigned int vtxIndex, int elementOffset, int elementDimension, int srcDimension, const float* src, bool isPos);
        error getVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int destDimension, float* dest, unsigned int count, bool isPos) const;
        error setVertexMemoryDataArray(unsigned int start, int elementOffset, int elementDimension,
            int srcDimension, const float* src, unsigned int count, bool isPos);

        void assembleNonVertexAttributes(const std::shared_ptr<GeometryAssembler>& assembler) const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        GeometryId m_id;

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

        Engine::BoundingVolume m_geometryBound;
    };

    using GeometryDataPtr = std::shared_ptr<GeometryData>;
}

#endif // GEOMETRY_DATA_H
