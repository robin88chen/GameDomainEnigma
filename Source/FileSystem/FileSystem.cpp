#include "FileSystem.h"
#include "IFile.h"
#include "IMountPath.h"
#include "Platforms/MemoryMacro.h"
#include "StdioFile.h"
#include "StdMountPath.h"
#include "Platforms/PlatformLayerUtilities.h"
#include <cassert>
#include <iostream>
#include <future>

#undef CreateFile

using namespace Enigma::FileSystem;
using namespace Enigma::Platforms;

FileSystem* FileSystem::m_instance = nullptr;

FileSystem::FileSystem()
{
    assert(!m_instance);
    m_instance = this;
}

FileSystem::~FileSystem()
{
    Cleanup();
    m_instance = nullptr;
}

FileSystem* FileSystem::Create()
{
    if (m_instance) return m_instance;
    return new FileSystem();
}

/** 由 Create 建立物件， 而不是讓 Instance 可以建立物件 */
FileSystem* FileSystem::Instance()
{
    assert(m_instance);
    return m_instance;
}

IFilePtr FileSystem::OpenFile(const std::string& filename, const std::string& rw_options, const std::string& path_id)
{
    Debug::Printf("Open file %s at path %s called in thread %d\n", filename.c_str(), path_id.c_str(), std::this_thread::get_id());
    bool readonly = true;
    if (rw_options.find("w") != std::string::npos) readonly = false;
    if ((path_id.length() != 0) && (m_mountPathList.size()))
    {
        for (MountPathList::iterator iter = m_mountPathList.begin(); iter != m_mountPathList.end(); ++iter)
        {
            if (!((*iter)->EqualPathID(path_id))) continue;
            IFilePtr file = OpenMountedFile((*iter), filename, rw_options);
            if (file) return file;
        }
    }

    return OpenStdioFile("", filename, rw_options);
}

FutureFile FileSystem::AsyncOpenFile(const std::string& filename,
    const std::string& rw_options, const std::string& path_id)
{
    return std::async(std::launch::async, [=]() -> IFilePtr { return this->OpenFile(filename, rw_options, path_id); });
}

IFilePtr FileSystem::OpenFile(const Filename& filename, const std::string& rw_options)
{
    return OpenFile(filename.GetSubPathFileName(), rw_options, filename.GetMountPathID());
}

FutureFile FileSystem::AsyncOpenFile(const Filename& filename, const std::string& rw_options)
{
    return AsyncOpenFile(filename.GetSubPathFileName(), rw_options, filename.GetMountPathID());
}

IFilePtr FileSystem::OpenStdioFile(const std::string& filepath, const std::string& filename,
    const std::string& rw_option)
{
    std::string fullpath = StdMountPath::FixFullPath(filepath, filename);
    IFilePtr file = IFilePtr{ menew StdioFile(fullpath, rw_option) };
    error open_er = file->Open();
    if (open_er)
    {
        Debug::ErrorPrintf("Open stdio file %s error %s\n", filename.c_str(), open_er.message().c_str());
        return nullptr;
    }
    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.emplace_back(file);

    return file;
}

void FileSystem::CloseFile(const IFilePtr& file)
{
    if (!file) return;
    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.remove(file);

    file->Close();
    //EN_DELETE file;
}

IFilePtr FileSystem::OpenMountedFile(const IMountPathPtr& path,
    const std::string& filename, const std::string& rw_option)
{
    assert(path);

    IFilePtr file = IFilePtr{ path->CreateFile(filename, rw_option) };
    if (!file) return nullptr;
    error open_er = file->Open();
    if (open_er)
    {
        Debug::ErrorPrintf("Open mounted file %s error %s\n", filename.c_str(), open_er.message().c_str());
        return nullptr;
    }

    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.emplace_back(file);

    return file;
}

std::string FileSystem::FixToFullPath(const std::string& filepath, const std::string& filename)
{
    if (!filename.length()) return "";

    if (filepath.length() == 0)
    {
        return filename;
    }

    std::string fullpath = filepath + filename;
    return fullpath;
}

void FileSystem::Cleanup()
{
    RemoveAllMountPath();
    CloseAllOpenedFile();
}
void FileSystem::AddMountPath(const IMountPathPtr& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=] (const IMountPathPtr& mp)->bool { return mp->EqualMountPath(path.get()); });
    if (found != m_mountPathList.end())
    {
        return;
    }
    m_mountPathList.emplace_back(path);
}

void FileSystem::RemoveMountPath(const IMountPathPtr& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=] (const IMountPathPtr& mp)->bool { return mp->EqualMountPath(path.get()); });
    if (found != m_mountPathList.end())
    {
        m_mountPathList.erase(found);
    }
}

void FileSystem::RemoveAllMountPath()
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    m_mountPathList.clear();
}

std::string FileSystem::GetStdioFullPath(const std::string& filename, const std::string& path_id)
{
    if ((path_id.length() != 0) && (m_mountPathList.size()))
    {
        for (MountPathList::iterator iter = m_mountPathList.begin(); iter != m_mountPathList.end(); ++iter)
        {
            if (!((*iter)->EqualPathID(path_id))) continue;
            StdMountPath* path = dynamic_cast<StdMountPath*>((*iter).get());
            if (!path) continue;
            std::string fullpath = path->FixFullPath(filename);
            if (StdioFile::IsFileExisted(fullpath)) return fullpath;
        }
    }

    return StdMountPath::FixFullPath("", filename);
}

IMountPathPtr FileSystem::FindMountPath(const std::filesystem::path& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=](const IMountPathPtr& mp)->bool { return mp->EqualMountPath(path); });
    if (found != m_mountPathList.end())
    {
        return *found;
    }
    return nullptr;
}

std::list<IMountPathPtr> FileSystem::GetMountPathsWithPathID(const std::string& path_id)
{
    MountPathList paths_list;
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    for (auto mount_path : m_mountPathList)
    {
        if (mount_path->GetPathID() == path_id)
        {
            paths_list.emplace_back(mount_path);
        }
    }
    return paths_list;
}

void FileSystem::CloseAllOpenedFile()
{
    while (m_openedFileList.size() > 0)
    {
        IFilePtr file = m_openedFileList.front();
        CloseFile(file);
    }
}

