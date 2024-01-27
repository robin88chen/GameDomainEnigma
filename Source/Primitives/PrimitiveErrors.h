/*********************************************************************
 * \file   PrimitiveErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef PRIMITIVE_ERRORS_H
#define PRIMITIVE_ERRORS_H

#include <system_error>

#include "Primitive.h"

namespace Enigma::Primitives
{
    enum class ErrorCode
    {
        ok = 0,

        primitiveFactoryNotExists = 100,
        primitiveEntityAlreadyExists,
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
    struct is_error_code_enum<Enigma::Primitives::ErrorCode> : true_type {};
}


#endif // PRIMITIVE_ERRORS_H
