#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryMacro.h"
#include "StdMountPath.h"
#include "StdioFile.h"
#include <cassert>
#include <algorithm>
#include <string>
#undef CreateFile

using namespace Enigma::FileSystem;

StdMountPath::StdMountPath(const std::string& file_path, const std::string& path_id)
    : IMountPath(path_id)
{
    m_filePath = file_path;
    std::replace(m_filePath.begin(), m_filePath.end(), '\\', '/');
    if (m_filePath.length() > 0)
    {
        if ((m_filePath[m_filePath.length() - 1] != '/') &&
            (m_filePath[m_filePath.length() - 1] != '\\'))
        {
            m_filePath += "/";
        }
    }
}

StdMountPath::~StdMountPath()
{
}

IFile* StdMountPath::CreateFile(const std::string& filename, const ReadWriteOption& rw_option)
{
    std::string full_path = FixFullPath(m_filePath, filename);
    IFile* file = menew StdioFile(full_path, rw_option);

    return file;
}

bool StdMountPath::EqualMountPath(IMountPath* path)
{
    assert(path);
    if (!EqualPathID(path->GetPathID())) return false;
    StdMountPath* std_path = dynamic_cast<StdMountPath*>(path);
    if (!std_path) return false;
    if (std_path->m_filePath != m_filePath) return false;
    return true;
}

bool StdMountPath::EqualMountPath(const std::filesystem::path& path)
{
    std::filesystem::path f_path(m_filePath);
    return std::filesystem::equivalent(path, f_path);
}

bool StdMountPath::EqualMouthPath(const std::string& path)
{
    return EqualMountPath(std::filesystem::path{path});
}

std::string StdMountPath::FixFullPath(const std::string& filename)
{
    if (!filename.length()) return "";

    if (m_filePath.length() == 0)
    {
        return filename;
    }

    std::string fullpath = m_filePath + filename;
    std::replace(fullpath.begin(), fullpath.end(), '\\', '/');
    return fullpath;
}

std::string StdMountPath::FixFullPath(const std::string& filepath, const std::string& filename)
{
    if (!filename.length()) return "";

    if (filepath.length() == 0)
    {
        return filename;
    }

    std::string fullpath = filepath + filename;
    std::replace(fullpath.begin(), fullpath.end(), '\\', '/');
    return fullpath;
}
