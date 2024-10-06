/*********************************************************************
 * \file   RenderBufferBuildingPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_BUILDING_POLICIES_H
#define RENDER_BUFFER_BUILDING_POLICIES_H

#include "RenderBufferSignature.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <string>
#include <optional>

namespace Enigma::Engine
{
    struct RenderBufferPolicy
    {
        std::string m_renderBufferName;
        RenderBufferSignature m_signature;
        std::string m_vtxBufferName;
        std::string m_idxBufferName;
        unsigned int m_sizeofVertex;
        unsigned int m_vtxBufferSize;
        unsigned int m_idxBufferSize;
        std::optional<byte_buffer> m_vtxBuffer;
        std::optional<uint_buffer> m_idxBuffer;
        RenderBufferPolicy() : m_sizeofVertex(0), m_vtxBufferSize(0), m_idxBufferSize(0) {}
    };
}


#endif // RENDER_BUFFER_BUILDING_POLICIES_H
