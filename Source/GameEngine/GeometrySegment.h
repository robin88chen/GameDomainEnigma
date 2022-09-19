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
namespace Enigma::Engine
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
    };
    using GeometrySegmentVector = std::vector<GeometrySegment>;
}

#endif // GEOMETRY_SEGMENT_H
