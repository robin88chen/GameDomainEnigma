/*********************************************************************
 * \file   IDepthStencilSurface.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef DEPTH_STENCIL_SURFACE_INTERFACE_H
#define DEPTH_STENCIL_SURFACE_INTERFACE_H

#include <memory>
#include <system_error>
#include "GraphicAPITypes.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IBackSurface;
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    /** Depth Stencil Buffer class */
    class IDepthStencilSurface : public std::enable_shared_from_this<IDepthStencilSurface>
    {
    public:
        IDepthStencilSurface() : m_dimension{ 0, 0 } {};
        IDepthStencilSurface(const IDepthStencilSurface&) = delete;
        IDepthStencilSurface(IDepthStencilSurface&&) = delete;
        virtual ~IDepthStencilSurface() = default;
        IDepthStencilSurface& operator=(const IDepthStencilSurface&) = delete;
        IDepthStencilSurface& operator=(IDepthStencilSurface&&) = delete;

        /** get buffer format */
        virtual const GraphicFormat& GetFormat() { return m_format; };
        /** get dimension */
        virtual const MathLib::Dimension& GetDimension() { return m_dimension; }

        virtual error Resize(const MathLib::Dimension& dimension) = 0;
        virtual future_error AsyncResize(const MathLib::Dimension& dimension);

        /// Graphic API Egl need this to bind surfaces together
        virtual void MakeBackSurfaceRelated(const IBackSurfacePtr&) {};

    protected:
        MathLib::Dimension m_dimension;

        GraphicFormat m_format;
    };
    using IDepthStencilSurfacePtr = std::shared_ptr<IDepthStencilSurface>;
    using IDepthStencilSurfaceWeak = std::weak_ptr<IDepthStencilSurface>;
};


#endif // DEPTH_STENCIL_SURFACE_INTERFACE_H
