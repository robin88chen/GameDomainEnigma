/*********************************************************************
 * \file   BackSurfaceSpecification.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef BACK_SURFACE_SPECIFICATION_H
#define BACK_SURFACE_SPECIFICATION_H

#include "MathLib/AlgebraBasicTypes.h"
#include "GraphicAPITypes.h"
#include <string>
#include <vector>

namespace Enigma::Graphics
{
    class BackSurfaceSpecification
    {
    public:
        BackSurfaceSpecification(const std::string& name, const MathLib::Dimension<unsigned>& dimension, const GraphicFormat& format) : m_name(name), m_dimension(dimension), m_format(format) {}

        const std::string& name() const { return m_name; }
        const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        const GraphicFormat& format() const { return m_format; }

    protected:
        std::string m_name;
        MathLib::Dimension<unsigned> m_dimension;
        GraphicFormat m_format;
    };

    class MultiBackSurfaceSpecification
    {
    public:
        MultiBackSurfaceSpecification(const std::string& name, const MathLib::Dimension<unsigned>& dimension, unsigned surface_count, const std::vector<GraphicFormat>& formats) : m_name(name), m_dimension(dimension), m_surfaceCount(surface_count), m_formats(formats) {}

        const std::string& name() const { return m_name; }
        const MathLib::Dimension<unsigned>& dimension() const { return m_dimension; }
        unsigned surfaceCount() const { return m_surfaceCount; }
        const std::vector<GraphicFormat>& formats() const { return m_formats; }

    protected:
        std::string m_name;
        MathLib::Dimension<unsigned> m_dimension;
        unsigned m_surfaceCount;
        std::vector<GraphicFormat> m_formats;
    };
}

#endif // BACK_SURFACE_SPECIFICATION_H
