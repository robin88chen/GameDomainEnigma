/*********************************************************************
 * \file   PrefabErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PREFAB_ERRORS_H
#define PREFAB_ERRORS_H

#include <system_error>

namespace Enigma::Prefabs
{
    enum class ErrorCode
    {
        ok = 0,
        emptyPrefabPath,
        nullDeserializer,
        emptyPrefabs,
        invalidPrefab,
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
    struct is_error_code_enum<Enigma::Prefabs::ErrorCode> : true_type {};
}

#endif // PREFAB_ERRORS_H
