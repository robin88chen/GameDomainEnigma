/********************************************************************
 * \file   AssetErrors.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef ASSET_PACKAGE_ERRORS_H
#define ASSET_PACKAGE_ERRORS_H

#include <system_error>

namespace Enigma::AssetPackage
{
    enum class ErrorCode
    {
        OK = 0,
        EmptyFileName,
        FileOpenFail,
        FileReadFail,
        FileWriteFail,
        ReadSizeCheck,
        WriteSizeCheck,
        EmptyBuffer,
        EmptyKey,
        CompressFail,
        DecompressFail,
        ZeroSizeAsset,
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

    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

namespace std
{
    // let compiler know that ErrorCode is compatible with std::error_code
    template <>
    struct is_error_code_enum<Enigma::AssetPackage::ErrorCode> : true_type {};
}

#endif // ASSET_PACKAGE_ERRORS_H
