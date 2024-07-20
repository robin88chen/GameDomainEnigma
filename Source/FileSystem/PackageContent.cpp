#include "PackageContent.h"
#include "AssetPackage/AssetPackageFile.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "FileSystemErrors.h"
#include <cassert>

using namespace Enigma::FileSystem;

PackageContent::PackageContent() : IFile()
{
    m_size = 0;
}

PackageContent::PackageContent(const std::shared_ptr<AssetPackage::AssetPackageFile>& pack, const std::string& key_name)
    : IFile()
{
    m_packageFile = pack;
    m_keyName = key_name;
    if (!m_packageFile.expired())
    {
        m_fullPath = m_packageFile.lock()->getBaseFilename() + "/" + m_keyName;
    }
    m_size = 0;
}

PackageContent::~PackageContent()
{
}

std::optional<std::vector<unsigned char>> PackageContent::read(size_t offset, size_t size_request)
{
    assert(size_request > 0);
    Platforms::Debug::Printf("Read File in thread %d\n", std::this_thread::get_id());
    if (!IsValidContent()) return std::nullopt;
    if (m_cacheBuffer.empty())
    {
        ErrorCode er = RetrieveAssetContent();
        if (makeErrorCode(er)) return std::nullopt;
    }
    if (m_cacheBuffer.empty())
    {
        return std::nullopt;
    }
    size_t read_bytes = size_request;
    if (offset + size_request > m_size) read_bytes = m_size - offset;
    return std::vector<unsigned char>(m_cacheBuffer.cbegin() + offset, m_cacheBuffer.cbegin() + offset + read_bytes);
}

size_t PackageContent::write(size_t, const std::vector<unsigned char>&)
{
    assert(!"Write not supported on package content");
    return 0;
}

size_t PackageContent::size()
{
    if (m_size != 0) return m_size;
    isExisted();
    return m_size;
}

time_t PackageContent::filetime()
{
    if (!IsValidContent()) return 0;
    return m_packageFile.lock()->getAssetTimeStamp(m_keyName);
}

bool PackageContent::isExisted()
{
    if (!IsValidContent()) return false;
    if (m_size != 0) return true;
    size_t file_size = m_packageFile.lock()->getAssetOriginalSize(m_keyName);
    m_size = file_size;
    return m_size != 0;
}

error PackageContent::open()
{
    if (!isExisted()) return lastError();
    return ErrorCode::ok;
}

error PackageContent::close()
{
    if (!IsValidContent()) return lastError();
    m_size = 0;
    m_cacheBuffer.clear();
    m_packageFile.reset();
    return ErrorCode::ok;
}

bool PackageContent::IsValidContent()
{
    if (m_packageFile.expired())
    {
        makeErrorCode(ErrorCode::expiredPackage);
        return false;
    }
    if (m_keyName.length() == 0)
    {
        makeErrorCode(ErrorCode::emptyAssetKey);
        return false;
    }
    return true;
}

ErrorCode PackageContent::RetrieveAssetContent()
{
    m_size = 0;
    if (!IsValidContent()) return lastErrorCode();

    size_t asset_size = m_packageFile.lock()->getAssetOriginalSize(m_keyName);
    if (asset_size == 0) return ErrorCode::zeroSizeContent;
    auto buff = m_packageFile.lock()->tryRetrieveAssetToMemory(m_keyName);
    if (!buff)
    {
        return ErrorCode::retrieveContentFail;
    }
    m_cacheBuffer = buff.value();
    m_size = m_cacheBuffer.size();
    return ErrorCode::ok;
}