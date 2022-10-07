/*********************************************************************
 * \file   RendererErrors.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDERER_ERRORS_H
#define RENDERER_ERRORS_H

#include <system_error>

namespace Enigma::Renderer
{
    enum class ErrorCode
    {
        ok = 0,

        flipNotPrimary = 101,
        nullBackSurface,
        renderTargetAlreadyExisted,
        renderTargetNotExist,

        nullRenderBuffer,
        nullEffectMaterial,
        nullRenderElement,
    };
    class ErrorCategory : public std::error_category
    {
    public:
        std::string message(int err) const override;
        const char* name() const noexcept override;
        static const std::error_category& get()
        {
            return ms_category;
        }
    private:
        const static ErrorCategory ms_category;
    };

    extern std::error_code make_error_code(ErrorCode ec);
}

namespace std
{
    // let compiler know that ErrorCode is compatible with std::error_code
    template <>
    struct is_error_code_enum<Enigma::Renderer::ErrorCode> : true_type {};
}


#endif // RENDERER_ERRORS_H
