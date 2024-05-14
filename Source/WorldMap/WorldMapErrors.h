/*********************************************************************
 * \file   WorldMapErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef WORLD_MAP_ERRORS_H
#define WORLD_MAP_ERRORS_H

#include <system_error>

namespace Enigma::WorldMap
{
    enum class ErrorCode
    {
        ok = 0,

        emptyQuadVolumeList = 101,
        parentQuadNodeNotFound,
        parentQuadNodeHydrationFailed,
        childQuadNodeConstitutionFailed,

        quadRootAlreadyExists = 201,
        worldMapAlreadyExists,
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
    struct is_error_code_enum<Enigma::WorldMap::ErrorCode> : true_type {};
}


#endif // WORLD_MAP_ERRORS_H
