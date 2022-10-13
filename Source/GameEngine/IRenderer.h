/*********************************************************************
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
        IRenderer(const std::string& name) : m_name(name) {};
        virtual ~IRenderer() {};

        /** get renderer's name */
        const std::string& GetName() { return m_name; };

    protected:
        std::string m_name;
    };
    using IRendererPtr = std::shared_ptr<IRenderer>;
}

#endif // RENDERER_INTERFACE_H
