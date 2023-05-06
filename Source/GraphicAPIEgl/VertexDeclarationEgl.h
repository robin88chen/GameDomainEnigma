/*********************************************************************
 * \file   VertexDeclarationEgl.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_DECLARATION_EGL_H
#define VERTEX_DECLARATION_EGL_H

#include "GraphicKernel/IVertexDeclaration.h"
#include "GraphicKernel/VertexDescription.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <system_error>
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "GLES3/gl3.h"

namespace Enigma::Devices
{
    using error = std::error_code;
    /** vertex declaration dx11 */
    class VertexDeclarationEgl : public Graphics::IVertexDeclaration
    {
    public:
        struct LayoutElement
        {
            GLint m_size;
            GLenum m_type;
            GLint m_position;
        };
    public:
        VertexDeclarationEgl(const std::string& name, const std::string& data_vertex_format);
        VertexDeclarationEgl(const VertexDeclarationEgl&) = delete;
        VertexDeclarationEgl(VertexDeclarationEgl&&) = delete;
        virtual ~VertexDeclarationEgl() override;
        VertexDeclarationEgl& operator=(const VertexDeclarationEgl&) = delete;
        VertexDeclarationEgl& operator=(VertexDeclarationEgl&&) = delete;

        virtual bool IsMatched(const std::string& data_vertex_format, const Graphics::IVertexShaderPtr& vtx_shader) override;

        unsigned int GetElementCount() { return m_layouts.size(); }
        const std::vector<LayoutElement>& GetLayoutElements() const { return m_layouts; }
        GLsizei GetVertexSize() const { return m_vertexSize; }
    protected:
        void CreateVertexLayout(const Graphics::VertexDescription& vertex_desc);

    protected:
        std::vector<LayoutElement> m_layouts;
        GLsizei m_vertexSize;
    };

};

#endif
#endif // VERTEX_DECLARATION_EGL_H
