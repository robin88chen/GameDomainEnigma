/*********************************************************************
 * \file   IVertexDeclaration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_DECLARATION_INTERFACE_H
#define VERTEX_DECLARATION_INTERFACE_H

#include <string>
#include <memory>
#include "VertexDescription.h"

namespace Enigma::Graphics
{
    class IVertexShader;
    using IVertexShaderPtr = std::shared_ptr<IVertexShader>;

    /** vertex format declaration class */
    class IVertexDeclaration : public std::enable_shared_from_this<IVertexDeclaration>
    {
    public:
        IVertexDeclaration(const std::string& name, const std::string& data_vertex_format);
        IVertexDeclaration(const IVertexDeclaration&) = delete;
        IVertexDeclaration(IVertexDeclaration&&) = delete;
        virtual ~IVertexDeclaration();
        IVertexDeclaration& operator=(const IVertexDeclaration&) = delete;
        IVertexDeclaration& operator=(IVertexDeclaration&&) = delete;

        const std::string& GetName() { return m_name; }
        const std::string& GetFormatString() { return m_formatString; };

        const VertexFormatCode& GetVertexFormatCode() const { return m_dataVertexFormatCode; };

        /** is vertex format matched? */
        virtual bool IsMatched(const std::string& data_vertex_format, const IVertexShaderPtr& vtx_shader) = 0;

        unsigned int SizeofDataVertex() const { return (unsigned int)m_dataVertexFormatDesc.TotalVertexSize(); };

    protected:
        std::string m_name;
        std::string m_formatString;
        VertexFormatCode m_dataVertexFormatCode;
        VertexDescription m_dataVertexFormatDesc;
    };
    using IVertexDeclarationPtr = std::shared_ptr<IVertexDeclaration>;
};


#endif // VERTEX_DECLARATION_INTERFACE_H
