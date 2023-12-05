/********************************************************************
 * \file   IMountPath.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MOUNT_PATH_INTERFACE_H
#define _MOUNT_PATH_INTERFACE_H

#include "ReadWriteOption.h"
#include <string>
#include <memory>
#include <filesystem>
#undef CreateFile

namespace Enigma::FileSystem
{
    class IFile;
    class IMountPath
    {
    public:
        /** Mount Path interface \n
        @param path_id 像是變數名稱之類的字串，例如 "EXECUTABLE_PATH", "RESOURCE_PATH"
        */
        IMountPath(const std::string& path_id) { m_pathID = path_id; };
        virtual ~IMountPath() = default;

        virtual IFile* createFile(const std::string& filename, const ReadWriteOption& rw_option) = 0;

        virtual bool equalPathId(const std::string& path_id)
        {
            if ((path_id.length() == 0) && (m_pathID.length() == 0)) return true;
            if ((path_id.length() != 0) && (m_pathID.length() != 0) && (m_pathID == path_id)) return true;
            return false;
        }
        const std::string& getPathId() { return m_pathID; }
        virtual bool equalMountPath(IMountPath* path) = 0;
        virtual bool equalMountPath(const std::filesystem::path& path) = 0;
        virtual bool equalMountPath(const std::string& path) = 0;

    protected:
        std::string m_pathID;
    };
    using IMountPathPtr = std::shared_ptr<IMountPath>;
}

#endif // !_MOUNT_PATH_INTERFACE_H
