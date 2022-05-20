#include "AssetErrors.h"

using namespace Enigma::AssetPackage;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::emptyFileName: return "Empty file name";
    case ErrorCode::fileOpenFail: return "File open fail";
    case ErrorCode::fileReadFail: return "File read fail";
    case ErrorCode::fileWriteFail: return "File write fail";
    case ErrorCode::fileSizeError: return "File size error";
    case ErrorCode::readSizeCheck: return "Read size check fail";
    case ErrorCode::writeSizeCheck: return "Write size check fail";
    case ErrorCode::emptyBuffer: return "Empty buffer";
    case ErrorCode::emptyKey: return "Empty key";
    case ErrorCode::compressFail: return "Compress fail";
    case ErrorCode::decompressFail: return "Decompress fail";
    case ErrorCode::zeroSizeAsset: return "Zero size asset error";
    case ErrorCode::assetSizeError: return "Asset size error";
    case ErrorCode::invalidHeaderData: return "Invalid header data";
    case ErrorCode::invalidNameList: return "Invalid name list";
    case ErrorCode::emptyHeader: return "Empty header";
    case ErrorCode::emptyNameList: return "Empty name list";
    case ErrorCode::duplicatedKey: return "Duplicated asset key";
    case ErrorCode::notExistedKey: return "Not existed asset key";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "AssetPacket Error Category";
}
namespace Enigma::AssetPackage
{
    inline std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}