/*********************************************************************
 * \file   GeometrySegment.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef GEOMETRY_SEGMENT_H
#define GEOMETRY_SEGMENT_H

#include <vector>
namespace Enigma::Geometries
{
    struct GeometrySegment
    {
        unsigned int m_startVtx;
        unsigned int m_vtxCount;
        unsigned int m_startIdx;
        unsigned int m_idxCount;
        GeometrySegment()
        {
            m_startVtx = m_vtxCount = m_startIdx = m_idxCount = 0;
        };
        GeometrySegment(unsigned int start_vtx, unsigned int vtx_count, unsigned int start_idx, unsigned int idx_count)
            : m_startVtx(start_vtx), m_vtxCount(vtx_count), m_startIdx(start_idx), m_idxCount(idx_count) {}
    };
    using GeometrySegmentVector = std::vector<GeometrySegment>;
}

#endif // GEOMETRY_SEGMENT_H
