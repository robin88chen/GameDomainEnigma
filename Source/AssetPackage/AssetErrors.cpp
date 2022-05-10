#include "AssetErrors.h"

using namespace Enigma::AssetPackage;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::OK: return "OK";
    case ErrorCode::EmptyFileName: return "Empty file name";
    case ErrorCode::FileOpenFail: return "File open fail";
    case ErrorCode::FileReadFail: return "File read fail";
    case ErrorCode::FileWriteFail: return "File write fail";
    case ErrorCode::ReadSizeCheck: return "Read size check fail";
    case ErrorCode::WriteSizeCheck: return "Write size check fail";
    case ErrorCode::EmptyBuffer: return "Empty buffer";
    case ErrorCode::EmptyKey: return "Empty key";
    case ErrorCode::CompressFail: return "Compress fail";
    case ErrorCode::DecompressFail: return "Decompress fail";
    case ErrorCode::ZeroSizeAsset: return "Zero size asset error";
    case ErrorCode::InvalidHeaderData: return "Invalid header data";
    case ErrorCode::InvalidNameList: return "Invalid name list";
    case ErrorCode::DuplicatedKey: return "Duplicated asset key";
    case ErrorCode::NotExistedKey: return "Not existed asset key";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "AssetPacket Error Category";
}
namespace Enigma::AssetPackage
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}