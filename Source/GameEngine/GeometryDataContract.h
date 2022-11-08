/*********************************************************************
 * \file   GeometryDataContract.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_CONTRACT_H
#define _GEOMETRY_DATA_CONTRACT_H

#include <string>
#include <vector>
#include "BoundingVolumeContract.h"
#include "Frameworks/ExtentTypesDefine.h"

namespace Enigma::Engine
{
    class GeometryDataContract
    {
    public:
        GeometryDataContract();

    protected:
        std::string m_name;
        std::string m_vertexFormat;
        std::vector<unsigned> m_segments;
        byte_buffer m_vertices;
        uint_buffer m_indices;
        unsigned m_vtxCapacity;
        unsigned m_idxCapacity;
        unsigned m_vtxUsedCount;
        unsigned m_idxUsedCount;
        unsigned m_topology;
        Contract m_geometryBound;
    };
}

#endif // _GEOMETRY_DATA_CONTRACT_H
