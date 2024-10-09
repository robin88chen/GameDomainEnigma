﻿/*********************************************************************
 * \file   TriangleList.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _TRIANGLE_LIST_H
#define _TRIANGLE_LIST_H

#include "GeometryData.h"
#include "GeometryId.h"
#include <memory>

namespace Enigma::Geometries
{
    using error = std::error_code;

    class TriangleList : public GeometryData
    {
        DECLARE_EN_RTTI;
    public:
        TriangleList(const GeometryId& id);
        TriangleList(const TriangleList&) = delete;
        TriangleList(TriangleList&&) = delete;
        virtual ~TriangleList() override;
        TriangleList& operator=(const TriangleList&) = delete;
        TriangleList& operator=(TriangleList&&) = delete;

        static std::shared_ptr<TriangleList> create(const GeometryId& id);

        virtual std::shared_ptr<GeometryAssembler> assembler() const override;
        virtual std::shared_ptr<GeometryDisassembler> disassembler() override;

        unsigned int getTriangleCount();
        void fetchTrianglePos(unsigned int idx, MathLib::Vector3 tri[3]);
        void fetchTriangleTextureCoord(unsigned int idx, unsigned int tex_channel, MathLib::Vector2 uv[3]);
        void fetchTriangleVertexIndex(unsigned int idx, unsigned int vtx_idx[3]);

        /** calculate tangent space */
        error calculateVertexTangentSpace(unsigned int tex_channel);
    };
    using TriangleListPtr = std::shared_ptr<TriangleList>;

}
#endif // _TRIANGLE_LIST_H
