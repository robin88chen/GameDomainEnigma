/********************************************************************
 * \file   PackageMountPath.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PACKAGE_MOUNT_PATH_H
#define _PACKAGE_MOUNT_PATH_H

#include "IMountPath.h"
#include <memory>

namespace Enigma::AssetPackage
{
    class AssetPackageFile;
}
namespace Enigma::FileSystem
{
    class PackageMountPath : public IMountPath
    {
    public:
        /** Search Path \n
        @param package 封裝檔物件
        @param path_id 像是變數名稱之類的字串，例如 "EXECUTABLE_PATH", "RESOURCE_PATH"
        */
        PackageMountPath(const std::shared_ptr<AssetPackage::AssetPackageFile>& package, const std::string& path_id);

        virtual ~PackageMountPath();

        virtual IFile* CreateFile(const std::string& filename, const std::string& rw_option) override;

        virtual bool EqualMountPath(IMountPath* path) override;
        virtual bool EqualMountPath(const std::filesystem::path& path) override;
        virtual bool EqualMouthPath(const std::string& path) override;

    protected:
        std::shared_ptr<AssetPackage::AssetPackageFile> m_assetPackage;
        std::string m_packageFilename;
    };
}

#endif // !_PACKAGE_MOUNT_PATH_H
