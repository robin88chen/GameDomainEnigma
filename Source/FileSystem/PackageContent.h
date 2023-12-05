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

        virtual std::string getFullPath() override { return m_fullPath; };

        virtual std::optional<std::vector<unsigned char>> read(size_t offset, size_t size_request) override;
        virtual size_t  write(size_t offset, const std::vector<unsigned char>& in_buff) override;

        virtual size_t size() override;

        virtual time_t filetime() override;

        virtual bool isExisted() override;
        virtual bool isWritable() override { return false; };

    protected:
        virtual error open() override;
        virtual error close() override;

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
