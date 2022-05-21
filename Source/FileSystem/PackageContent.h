/********************************************************************
 * \file   PackageContent.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _PACKAGE_CONTENT_H
#define _PACKAGE_CONTENT_H

#include "IFile.h"

namespace Enigma::AssetPackage
{
    class AssetPackageFile;
}

namespace Enigma::FileSystem
{
    class PackageContent : public IFile
    {
    public:
        PackageContent();
        PackageContent(const PackageContent&) = delete;
        PackageContent(const std::shared_ptr<AssetPackage::AssetPackageFile>& pack, const std::string& key_name);
        virtual ~PackageContent();
        PackageContent& operator=(const PackageContent&) = delete;

        virtual std::string GetFullPath() override { return m_fullPath; };

        virtual std::optional<std::vector<char>> Read(size_t offset, size_t size_request) override;
        virtual size_t  Write(size_t offset, void const* in_buff, size_t size) override;

        virtual size_t Size() override;

        virtual time_t FileTime() override;

        virtual bool IsExisted() override;
        virtual bool IsWritable() override { return false; };

    protected:
        virtual error Open() override;
        virtual error Close() override;

        bool IsValidContent();

        ErrorCode RetrieveAssetContent();
    private:
        std::weak_ptr<AssetPackage::AssetPackageFile> m_packageFile;
        std::string m_keyName;
        std::string m_fullPath;

        std::vector<char> m_cacheBuffer;
        size_t m_size;
    };
}
#endif // !_PACKAGE_CONTENT_H
