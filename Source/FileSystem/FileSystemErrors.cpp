#include "FileSystemErrors.h"

using namespace Enigma::FileSystem;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::emptyFilePath: return "Empty file path";
    case ErrorCode::emptyRWOption: return "Empty R/W Option";
    case ErrorCode::nullAndroidAsset: return "Null android asset";
    case ErrorCode::androidAssetManagerError: return "Android asset manager error";
    case ErrorCode::fileOpenError: return "File open error";
    case ErrorCode::fileStatusError: return "File status error";
    case ErrorCode::expiredPackage: return "Expired package";
    case ErrorCode::emptyAssetKey: return "Empty asset key";
    case ErrorCode::emptyWriteBuffer: return "Empty write buffer";
    case ErrorCode::zeroReadSize: return "Zero read size";
    case ErrorCode::zeroSizeContent: return "Zero size content";
    case ErrorCode::readOffsetError: return "Read offset error";
    case ErrorCode::readFail: return "Read fail";
    case ErrorCode::writeFail: return "Write fail";
    case ErrorCode::retrieveContentFail: return "Retrieve content fail";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "FileSystem Error Category";
}
namespace Enigma::FileSystem
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}