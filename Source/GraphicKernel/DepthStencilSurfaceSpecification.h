/*********************************************************************
 * \file   DepthStencilSurfaceSpecification.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef DEPTH_STENCIL_SURFACE_SPECIFICATION_H
#define DEPTH_STENCIL_SURFACE_SPECIFICATION_H

#include "MathLib/AlgebraBasicTypes.h"
#include "GraphicAPITypes.h"
#include <string>

namespace Enigma::Graphics
{
    class DepthStencilSurfaceSpecification
    {
    public:
        DepthStencilSurfaceSpecification(const std::string& name, const MathLib::Dimension<unsigned>& dimension, const GraphicFormat& format) : m_name(name), m_dimension(dimension), m_format(format) {}

        const std::string& name() const { return m_name; }
        const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        const GraphicFormat& format() const { return m_format; }

    protected:
        std::string m_name;
        MathLib::Dimension<unsigned> m_dimension;
        GraphicFormat m_format;
    };
}

#endif // DEPTH_STENCIL_SURFACE_SPECIFICATION_H
