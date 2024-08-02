﻿/*********************************************************************
 * \file   RenderingErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef RENDERING_ERRORS_H
#define RENDERING_ERRORS_H

#include <system_error>

namespace Enigma::Rendering
{
    enum class ErrorCode
    {
        ok = 0,
        nullHostLight,
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
    struct is_error_code_enum<Enigma::Rendering::ErrorCode> : true_type {};
}

#endif // RENDERING_ERRORS_H
