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
    cleanup();
    m_instance = nullptr;
}

FileSystem* FileSystem::create()
{
    if (m_instance) return m_instance;
    return new FileSystem();
}

/** 由 Create 建立物件， 而不是讓 Instance 可以建立物件 */
FileSystem* FileSystem::instance()
{
    assert(m_instance);
    return m_instance;
}

IFilePtr FileSystem::openFile(const std::string& filename, const ReadWriteOption& rw_options, const std::string& path_id)
{
    Debug::Printf("Open file %s at path %s called in thread %d\n", filename.c_str(), path_id.c_str(), std::this_thread::get_id());
    bool readonly = true;
    if ((rw_options & ReadWriteOptionWrite).any()) readonly = false;
    if ((path_id.length() != 0) && (m_mountPathList.size()))
    {
        for (MountPathList::iterator iter = m_mountPathList.begin(); iter != m_mountPathList.end(); ++iter)
        {
            if (!((*iter)->equalPathId(path_id))) continue;
            IFilePtr file = openMountedFile((*iter), filename, rw_options);
            if (file) return file;
        }
    }

    return openStdioFile("", filename, rw_options);
}

FutureFile FileSystem::asyncOpenFile(const std::string& filename,
    const ReadWriteOption& rw_options, const std::string& path_id)
{
    return std::async(std::launch::async, [=]() -> IFilePtr { return this->openFile(filename, rw_options, path_id); });
}

IFilePtr FileSystem::openFile(const Filename& filename, const ReadWriteOption& rw_options)
{
    return openFile(filename.getSubPathFileName(), rw_options, filename.getMountPathId());
}

FutureFile FileSystem::asyncOpenFile(const Filename& filename, const ReadWriteOption& rw_options)
{
    return asyncOpenFile(filename.getSubPathFileName(), rw_options, filename.getMountPathId());
}

IFilePtr FileSystem::openStdioFile(const std::string& filepath, const std::string& filename,
    const ReadWriteOption& rw_option)
{
    std::string fullpath = StdMountPath::fixFullPath(filepath, filename);
    IFilePtr file = IFilePtr{ menew StdioFile(fullpath, rw_option) };
    error open_er = file->open();
    if (open_er)
    {
        Debug::ErrorPrintf("Open stdio file %s error %s\n", filename.c_str(), open_er.message().c_str());
        return nullptr;
    }
    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.emplace_back(file);

    return file;
}

void FileSystem::closeFile(const IFilePtr& file)
{
    if (!file) return;
    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.remove(file);

    file->close();
    //EN_DELETE file;
}

IFilePtr FileSystem::openMountedFile(const IMountPathPtr& path,
    const std::string& filename, const ReadWriteOption& rw_option)
{
    assert(path);

    IFilePtr file = IFilePtr{ path->createFile(filename, rw_option) };
    if (!file) return nullptr;
    error open_er = file->open();
    if (open_er)
    {
        Debug::ErrorPrintf("Open mounted file %s error %s\n", filename.c_str(), open_er.message().c_str());
        return nullptr;
    }

    std::lock_guard<std::mutex> locker{ m_openedFileLocker };
    m_openedFileList.emplace_back(file);

    return file;
}

std::string FileSystem::fixToFullPath(const std::string& filepath, const std::string& filename)
{
    if (!filename.length()) return "";

    if (filepath.length() == 0)
    {
        return filename;
    }

    std::string fullpath = filepath + filename;
    return fullpath;
}

void FileSystem::cleanup()
{
    removeAllMountPaths();
    closeAllOpenedFiles();
}
void FileSystem::addMountPath(const IMountPathPtr& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=](const IMountPathPtr& mp)->bool { return mp->equalMountPath(path.get()); });
    if (found != m_mountPathList.end())
    {
        return;
    }
    m_mountPathList.emplace_back(path);
}

void FileSystem::removeMountPath(const IMountPathPtr& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=](const IMountPathPtr& mp)->bool { return mp->equalMountPath(path.get()); });
    if (found != m_mountPathList.end())
    {
        m_mountPathList.erase(found);
    }
}

void FileSystem::removeAllMountPaths()
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    m_mountPathList.clear();
}

std::string FileSystem::getStdioFullPath(const std::string& filename, const std::string& path_id)
{
    if ((path_id.length() != 0) && (m_mountPathList.size()))
    {
        for (MountPathList::iterator iter = m_mountPathList.begin(); iter != m_mountPathList.end(); ++iter)
        {
            if (!((*iter)->equalPathId(path_id))) continue;
            StdMountPath* path = dynamic_cast<StdMountPath*>((*iter).get());
            if (!path) continue;
            std::string fullpath = path->fixFullPath(filename);
            if (StdioFile::isFileExisted(fullpath)) return fullpath;
        }
    }

    return StdMountPath::fixFullPath("", filename);
}

IMountPathPtr FileSystem::findMountPath(const std::filesystem::path& path)
{
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    MountPathList::iterator found = find_if(m_mountPathList.begin(), m_mountPathList.end(),
        [=](const IMountPathPtr& mp)->bool { return mp->equalMountPath(path); });
    if (found != m_mountPathList.end())
    {
        return *found;
    }
    return nullptr;
}

std::list<IMountPathPtr> FileSystem::getMountPathsWithPathId(const std::string& path_id)
{
    MountPathList paths_list;
    std::lock_guard<std::mutex> path_locker{ m_mountPathLocker };
    for (auto mount_path : m_mountPathList)
    {
        if (mount_path->getPathId() == path_id)
        {
            paths_list.emplace_back(mount_path);
        }
    }
    return paths_list;
}

void FileSystem::closeAllOpenedFiles()
{
    while (m_openedFileList.size() > 0)
    {
        IFilePtr file = m_openedFileList.front();
        closeFile(file);
    }
}

