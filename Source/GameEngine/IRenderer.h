﻿/*********************************************************************
 * \file   IRenderer.h
 * \brief  Renderer interface, implement in renderer layer
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDERER_INTERFACE_H
#define RENDERER_INTERFACE_H

#include <memory>
#include <string>

namespace Enigma::Engine
{
    class IRenderer : public std::enable_shared_from_this<IRenderer>
    {
    public:
        IRenderer(const std::string& name) : m_name(name), m_stampBitMask(0) {};
        IRenderer(const IRenderer&) = delete;
        IRenderer(IRenderer&&) = delete;
        virtual ~IRenderer() {};
        IRenderer& operator=(const IRenderer&) = delete;
        IRenderer& operator=(IRenderer&&) = delete;

        /** get renderer's name */
        const std::string& name() { return m_name; };

        /** set stamp bit mask */
        void stampBitMask(unsigned int mask) { m_stampBitMask = mask; };
        /** get stamp bit mask */
        unsigned int stampBitMask() const { return m_stampBitMask; };

    protected:
        std::string m_name;

        /** renderer bit mask, 每個renderer各用不同的bit, render element用以記錄放在哪個renderer中 */
        unsigned int m_stampBitMask;
    };
    using IRendererPtr = std::shared_ptr<IRenderer>;
}

#endif // RENDERER_INTERFACE_H
