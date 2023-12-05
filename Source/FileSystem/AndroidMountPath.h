/*********************************************************************
 * \file   AndroidMountPath.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _ANDROID_MOUNT_PATH_H
#define _ANDROID_MOUNT_PATH_H

#include "IMountPath.h"

namespace Enigma::FileSystem
{
    class AndroidMountPath : public IMountPath
    {
    public:
        /** Search Path \n
        @param sub_path 在 /assets/ 下的子路徑
        @param path_id 像是變數名稱之類的字串，例如 "EXECUTABLE_PATH", "RESOURCE_PATH"
        */
        AndroidMountPath(const std::string& sub_path, const std::string& path_id);

        virtual ~AndroidMountPath();

        virtual IFile* createFile(const std::string& filename, const ReadWriteOption& rw_option) override;

        virtual bool equalMountPath(IMountPath* path) override;
        virtual bool equalMountPath(const std::filesystem::path& path) override;
        virtual bool equalMountPath(const std::string& path) override;

    protected:
        std::string m_subPath;
    };
}

#endif // !_ANDROID_MOUNT_PATH_H
