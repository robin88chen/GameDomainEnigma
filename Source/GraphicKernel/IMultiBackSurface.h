/*********************************************************************
 * \file   IMultiBackSurface.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef MULTI_BACKSURFACE_INTERFACE_H
#define MULTI_BACKSURFACE_INTERFACE_H

#include "IBackSurface.h"

namespace Enigma::Graphics
{
    class IMultiBackSurface : public IBackSurface
    {
    public:
        IMultiBackSurface(const std::string& name);
        IMultiBackSurface(const IMultiBackSurface&) = delete;
        virtual ~IMultiBackSurface();
        IMultiBackSurface& operator=(const IMultiBackSurface&) = delete;

        virtual error Resize(const MathLib::Dimension& dimension) override = 0;

        GraphicFormat GetSliceFormat(unsigned int index);
        virtual unsigned int GetSurfaceCount() override;

        virtual bool IsMultiSurface() override { return true; }

    protected:
        unsigned int m_surfaceCount;
        std::vector<GraphicFormat> m_formatArray;
    };
    using IMultiBackSurfacePtr = std::shared_ptr<IMultiBackSurface>;
    using IMultiBackSurfaceWeak = std::weak_ptr<IMultiBackSurface>;
}


#endif // MULTI_BACKSURFACE_INTERFACE_H
