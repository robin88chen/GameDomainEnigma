/*********************************************************************
 * \file   RenderablePrimitiveResponses.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_RESPONSES_H
#define _RENDERABLE_PRIMITIVE_RESPONSES_H

#include "Frameworks/Response.h"
#include "GameEngine/Primitive.h"
#include <system_error>

namespace Enigma::Renderer
{
    class BuildRenderablePrimitiveResponse : public Frameworks::IResponse
    {
    public:
        BuildRenderablePrimitiveResponse(const Frameworks::Ruid& request_ruid,
            const std::string& name, const Engine::PrimitivePtr prim, std::error_code er) : IResponse(request_ruid),
           m_name(name), m_prim(prim), m_error(er) {};

        const std::string& getName() { return m_name; }
        const Engine::PrimitivePtr& GetPrimitive() { return m_prim; }
        std::error_code GetErrorCode() const { return m_error; }

    private:
        std::string m_name;
        Engine::PrimitivePtr m_prim;
        std::error_code m_error;
    };
}

#endif // _RENDERABLE_PRIMITIVE_RESPONSES_H
