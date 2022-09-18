/*********************************************************************
 * \file   FileSystem.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "IFile.h"
#include "IMountPath.h"
#include "Filename.h"
#include <string>
#include <list>
#include <mutex>
#include <filesystem>

namespace Enigma::FileSystem
{
    class FileSystem
    {
    public:
        ~FileSystem();

        /** 以 create 函式來建立 file system */
        static FileSystem* Create();
        static FileSystem* Instance();

        /** Open File
        @param rw_options r: read, w: write, +: open always, m: multi-thread
        @param path_id  像是變數名稱之類的字串，例如 "EXECUTABLE_PATH", "RESOURCE_PATH" */
        IFilePtr OpenFile(const std::string& filename, const std::string& rw_options, const std::string& path_id);
        FutureFile AsyncOpenFile(const std::string& filename, const std::string& rw_options, const std::string& path_id);

    	/** Open File
        @param rw_options r: read, w: write, +: open always, m: multi-thread
        @param filename  filename object */
        IFilePtr OpenFile(const Filename& filename, const std::string& rw_options);
        FutureFile AsyncOpenFile(const Filename& filename, const std::string& rw_options);

        void CloseFile(const IFilePtr& file);

        /** Add Mount Path \n 相同的PathID可以增加多次, 會依序搜尋  */
        void AddMountPath(const IMountPathPtr& path);
        void RemoveMountPath(const IMountPathPtr& path);

        /** 從 path id 取得 full path */
        std::string GetStdioFullPath(const std::string& filename, const std::string& path_id);

        IMountPathPtr FindMountPath(const std::filesystem::path& path);
        std::list<IMountPathPtr> GetMountPathsWithPathID(const std::string& path_id);

    private:
        FileSystem();
        void Cleanup();
        void CloseAllOpenedFile();
        void RemoveAllMountPath();

        IFilePtr OpenStdioFile(const std::string& filepath, const std::string& filename, const std::string& rw_option);

        IFilePtr OpenMountedFile(const IMountPathPtr& path, const std::string& filename, const std::string& rw_option);
        std::string FixToFullPath(const std::string& filepath, const std::string& filename);

    private:
        static FileSystem* m_instance;

        using FileObjectList = std::list<IFilePtr>;
        using MountPathList = std::list<IMountPathPtr>;

        FileObjectList m_openedFileList;
        std::mutex m_openedFileLocker;
        MountPathList m_mountPathList;
        std::mutex m_mountPathLocker;
    };
};

#endif // !_FILE_SYSTEM_H
