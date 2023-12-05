#include "AndroidMountPath.h"
#include "AndroidAsset.h"
#include "Platforms/MemoryMacro.h"
#include <cassert>

using namespace Enigma::FileSystem;

AndroidMountPath::AndroidMountPath(const std::string& sub_path, const std::string& path_id)
    : IMountPath(path_id)
{
    m_subPath = sub_path;
    if (m_subPath.length() > 0)
    {
        if (m_subPath[m_subPath.length() - 1] != '/')
        {
            m_subPath += "/";
        }
    }
}

AndroidMountPath::~AndroidMountPath()
{
}

#pragma warning(disable:4100)  // for unused parameter rw_option in VC++
IFile* AndroidMountPath::createFile(const std::string& filename, const ReadWriteOption& rw_option)
{
    if (filename.length() == 0) return nullptr;
    IFile* file = nullptr;
#if TARGET_PLATFORM == PLATFORM_ANDROID
    file = menew AndroidAsset(m_subPath + filename, rw_option);
#endif
    return file;
}
#pragma warning(default:4100)

bool AndroidMountPath::equalMountPath(IMountPath* path)
{
    assert(path);
    if (!equalPathId(path->getPathId())) return false;
    AndroidMountPath* pkg_path = dynamic_cast<AndroidMountPath*>(path);
    if (!pkg_path) return false;
    if (pkg_path->m_subPath != m_subPath) return false;
    return true;
}

bool AndroidMountPath::equalMountPath(const std::filesystem::path& path)
{
    return m_subPath == path.string();
}

bool AndroidMountPath::equalMountPath(const std::string& path)
{
    return m_subPath == path;
}
