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
        FileSizeError,
        ReadSizeCheck,
        WriteSizeCheck,
        EmptyBuffer,
        EmptyKey,
        CompressFail,
        DecompressFail,
        ZeroSizeAsset,
        AssetSizeError,
        InvalidHeaderData,
        InvalidNameList,
        EmptyHeader,
        EmptyNameList,
        DuplicatedKey,
        NotExistedKey,
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
        static const std::error_code& last_error() 
        {
            return ms_last_error;
        }
        static void last_error(const std::error_code& er)
        {
            ms_last_error = er;
        }
    private:
        const static ErrorCategory ms_category;
        static std::error_code ms_last_error;
    };

    std::error_code make_error_code(ErrorCode ec);
    extern const std::error_code& last_error();
}

namespace std
{
    // let compiler know that ErrorCode is compatible with std::error_code
    template <>
    struct is_error_code_enum<Enigma::AssetPackage::ErrorCode> : true_type {};
}

#endif // ASSET_PACKAGE_ERRORS_H
