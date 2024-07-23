/*********************************************************************
 * \file   FileSystemErrors.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef FILE_SYSTEM_ERRORS_H
#define FILE_SYSTEM_ERRORS_H

#include <system_error>

namespace Enigma::FileSystem
{
    enum class ErrorCode
    {
        ok = 0,
        emptyFilePath,
        emptyRWOption,
        nullAndroidAsset,
        androidAssetManagerError,
        fileOpenError,
        fileStatusError,
        expiredPackage,
        emptyAssetKey,
        emptyWriteBuffer,
        zeroReadSize,
        zeroSizeContent,
        readOffsetError,
        readFail,
        writeFail,
        retrieveContentFail,
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
    struct is_error_code_enum<Enigma::FileSystem::ErrorCode> : true_type {};
}


#endif // FILE_SYSTEM_ERRORS_H
