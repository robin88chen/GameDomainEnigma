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
        IBackSurface(const std::string& name, bool primary) : m_name(name), m_dimension{ 0, 0 } { m_isPrimary = primary; };
        IBackSurface(const IBackSurface&) = delete;
        IBackSurface(IBackSurface&&) = delete;
        virtual ~IBackSurface() = default;
        IBackSurface& operator=(const IBackSurface&) = delete;
        IBackSurface& operator=(IBackSurface&&) = delete;

        const std::string& GetName() { return m_name; }
        virtual const GraphicFormat& GetFormat() { return m_format; };
        virtual const MathLib::Dimension& GetDimension() { return m_dimension; };

        virtual error Resize(const MathLib::Dimension& dimension) = 0;
        virtual future_error AsyncResize(const MathLib::Dimension& dimension);

        virtual bool IsPrimary() { return m_isPrimary; }

        virtual bool IsMultiSurface() { return false; }
        virtual unsigned int GetSurfaceCount() { return 1; };
    protected:
        std::string m_name;
        bool m_isPrimary;
        MathLib::Dimension m_dimension;
        GraphicFormat m_format;
    };
    using IBackSurfacePtr = std::shared_ptr<IBackSurface>;
    using IBackSurfaceWeak = std::weak_ptr<IBackSurface>;
}

#endif // BACK_SURFACE_INTERFACE_H
