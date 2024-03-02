/*********************************************************************
 * \file   IBackSurface.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef BACK_SURFACE_INTERFACE_H
#define BACK_SURFACE_INTERFACE_H

#include "GraphicAPITypes.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <memory>
#include <system_error>

namespace Enigma::Graphics
{
    using error = std::error_code;

    class IDepthStencilSurface;

    class IBackSurface : public std::enable_shared_from_this<IBackSurface>
    {
    public:
        IBackSurface(const std::string& name, bool primary);
        IBackSurface(const IBackSurface&) = delete;
        IBackSurface(IBackSurface&&) = delete;
        virtual ~IBackSurface();
        IBackSurface& operator=(const IBackSurface&) = delete;
        IBackSurface& operator=(IBackSurface&&) = delete;

        const std::string& getName() { return m_name; }
        virtual const GraphicFormat& GetFormat() { return m_format; };
        virtual const MathLib::Dimension<unsigned>& getDimension() { return m_dimension; };

        virtual bool IsPrimary() { return m_isPrimary; }

        virtual bool IsMultiSurface() { return false; }
        virtual unsigned int GetSurfaceCount() { return 1; };

        virtual void ResizeSurface(const MathLib::Dimension<unsigned>& dimension);

    protected:
        virtual error Resize(const MathLib::Dimension<unsigned>& dimension) = 0;
        virtual future_error AsyncResize(const MathLib::Dimension<unsigned>& dimension);

    protected:
        std::string m_name;
        bool m_isPrimary;
        MathLib::Dimension<unsigned> m_dimension;
        GraphicFormat m_format;
    };
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    using IBackSurfaceWeak = std::weak_ptr<IBackSurface>;
}

#endif // BACK_SURFACE_INTERFACE_H
