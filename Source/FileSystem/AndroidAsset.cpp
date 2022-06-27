#include "AndroidAsset.h"

#include "FileSystemErrors.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Platforms/AndroidBridge.h"
#include <cassert>

using namespace Enigma::FileSystem;
using namespace Enigma::Platforms;

std::mutex AndroidAsset::m_allAssetLocker;

//! Note for implemention:
//! NDK 的 AAsset 並沒有 thread-safe, 所以用了個全域的 mutex,
//! 所以, 物件內的成員函式不能互相呼叫, 以避免 Dead-lock
AndroidAsset::AndroidAsset()
{
    m_aasset = nullptr;
}

AndroidAsset::AndroidAsset(const std::string& filename, const std::string& rw_option)
{
    m_aasset = nullptr;
    m_filename = filename;
    m_fullPath = "assets/" + m_filename;
    m_rwOption = rw_option;
}

AndroidAsset::~AndroidAsset()
{
    Debug::Printf("Android asset destructor");
    Close();
}

std::optional<std::vector<unsigned char>> AndroidAsset::Read(size_t offset, size_t size_request)
{
    std::lock_guard<std::mutex> asset_locker{ m_allAssetLocker };
    Debug::Printf("Read File %s, %d in thread %d\n", m_filename.c_str(), (long)m_aasset,  std::this_thread::get_id());
    if (!m_aasset)
    {
        MakeErrorCode(ErrorCode::nullAndroidAsset);
        return std::nullopt;
    }
    size_t file_length = AAsset_getLength(m_aasset);
    if (offset > file_length)
    {
        MakeErrorCode(ErrorCode::readOffsetError);
        return std::nullopt;
    }
    if (offset + size_request > file_length) size_request = file_length - offset;

    off_t sp = AAsset_seek(m_aasset, offset, SEEK_SET);
    if (sp == (off_t)-1)
    {
        MakeErrorCode(ErrorCode::readOffsetError);
        return std::nullopt;
    }
    std::vector<unsigned char> out_buff;
    out_buff.resize(size_request, 0);
    int read_bytes = AAsset_read(m_aasset, &out_buff[0], size_request);
    if (read_bytes < 0)
    {
        MakeErrorCode(ErrorCode::readFail);
        return std::nullopt;
    }

    return out_buff;
}

size_t AndroidAsset::Write(size_t offset, const std::vector<unsigned char>& in_buff)
{
    assert(!"Write not supported on android asset");
    return 0;
}

size_t AndroidAsset::Size()
{
    std::lock_guard<std::mutex> asset_locker{ m_allAssetLocker };
    if (!m_aasset)
    {
        MakeErrorCode(ErrorCode::nullAndroidAsset);
        return 0;
    }
    return (size_t)AAsset_getLength(m_aasset);
}

bool AndroidAsset::IsExisted()
{
    if (!AndroidBridge::GetAAssetManager()) return false;
    if (m_filename.length() == 0) return false;

    std::lock_guard<std::mutex> asset_locker{ m_allAssetLocker };

    AAsset* aasset = AAssetManager_open(AndroidBridge::GetAAssetManager(), m_filename.c_str(), AASSET_MODE_UNKNOWN);
    if (!aasset) return false;
    AAsset_close(aasset);
    return true;
}

error AndroidAsset::Open()
{
    if (m_fullPath.length() == 0) return ErrorCode::emptyFilePath;
    if (m_rwOption.length() == 0) return ErrorCode::emptyRWOption;
    if (!AndroidBridge::GetAAssetManager()) return ErrorCode::androidAssetManagerError;
    std::lock_guard<std::mutex> asset_locker{ m_allAssetLocker };
    m_aasset = AAssetManager_open(AndroidBridge::GetAAssetManager(), m_filename.c_str(), AASSET_MODE_UNKNOWN);
    if (!m_aasset) return ErrorCode::fileOpenError;
    return ErrorCode::ok;
}

error AndroidAsset::Close()
{
    Debug::Printf("Close file %s\n", m_filename.c_str());
    std::lock_guard<std::mutex> asset_locker{ m_allAssetLocker };
    if (m_aasset != nullptr)
    {
        AAsset_close(m_aasset);
        m_aasset = nullptr;
    }
    return ErrorCode::ok;
}
#endif