/*********************************************************************
 * \file   StdMountPath.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _STD_MOUNT_PATH_H
#define _STD_MOUNT_PATH_H

#include "IMountPath.h"

namespace Enigma::FileSystem
{
    class StdMountPath : public IMountPath
    {
    public:
        /** Mount Path \n
        @param file_path OS的檔案系統路徑
        @param path_id 像是變數名稱之類的字串，例如 "EXECUTABLE_PATH", "RESOURCE_PATH"
        */
        StdMountPath(const std::string& file_path, const std::string& path_id);
        StdMountPath(const StdMountPath&) = delete;
        StdMountPath(StdMountPath&&) = delete;
        virtual ~StdMountPath();

        StdMountPath& operator=(const StdMountPath&) = delete;
        StdMountPath& operator=(StdMountPath&&) = delete;

        virtual IFile* CreateFile(const std::string& filename, const std::string& rw_option) override;

        virtual bool EqualMountPath(IMountPath* path) override;
        virtual bool EqualMountPath(const std::filesystem::path& path) override;
        virtual bool EqualMouthPath(const std::string& path) override;

        std::string FixFullPath(const std::string& filename);

        static std::string FixFullPath(const std::string& filepath, const std::string& filename);

    protected:
        std::string m_filePath;
    };
}

#endif // !_STD_MOUNT_PATH_H
