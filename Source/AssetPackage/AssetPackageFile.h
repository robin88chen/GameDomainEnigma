/********************************************************************
 * \file   AssetPackageFile.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _ASSET_PACKAGE_FILE_H
#define _ASSET_PACKAGE_FILE_H

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <fstream>
#include "AssetHeaderDataMap.h"
#include <mutex>

namespace Enigma::AssetPackage
{
    class AssetNameList;
    class AssetHashTable;

    using error = std::error_code;
    class AssetPackageFile
    {
    public:
        constexpr static unsigned int VERSION_USE_FILE_TIME = 0;
    public:
        AssetPackageFile(const AssetPackageFile&) = delete;
        AssetPackageFile(AssetPackageFile&&) = delete;
        ~AssetPackageFile();
        AssetPackageFile& operator=(const AssetPackageFile&) = delete;
        AssetPackageFile& operator=(AssetPackageFile&&) = delete;

        const std::string& getBaseFilename() { return m_baseFilename; };
        static AssetPackageFile* createNewPackage(const std::string& basefilename);
        static AssetPackageFile* openPackage(const std::string& basefilename);

        error addAssetFile(const std::string& file_path, const std::string& asset_key, unsigned int version);
        error addAssetMemory(const std::vector<char>& buff, const std::string& asset_key, unsigned int version);
        error tryRetrieveAssetToFile(const std::string& file_path, const std::string& asset_key);
        std::optional<std::vector<char>> tryRetrieveAssetToMemory(const std::string& asset_key);
        unsigned int getAssetOriginalSize(const std::string& asset_key);
        time_t getAssetTimeStamp(const std::string& asset_key);

        error removeAsset(const std::string& asset_key);

        const std::unique_ptr<AssetNameList>& getAssetNameList() { return m_nameList; };
        std::optional<AssetHeaderDataMap::AssetHeaderData> tryGetAssetHeaderData(const std::string& asset_key) const;
    private:
        AssetPackageFile();
        error createNewPackageImp(const std::string& basefilename);
        error openPackageImp(const std::string& basefilename);
        void resetPackage();

        void saveHeaderFile();
        void readHeaderFile();

        std::tuple<std::vector<char>, unsigned int> readBundleContent(unsigned int offset, unsigned int content_size);
        error repackBundleContent(const unsigned int content_size, const unsigned int base_offset);

    private:
        unsigned int m_formatTag;
        unsigned int m_fileVersion;
        unsigned int m_assetCount;
        std::unique_ptr<AssetNameList> m_nameList;
        std::unique_ptr<AssetHeaderDataMap> m_headerDataMap;

        std::string m_baseFilename;
        std::fstream m_headerFile;
        std::fstream m_bundleFile;

        std::mutex m_headerFileLocker;
        std::mutex m_bundleFileLocker;
    };

    using AssetPackageFilePtr = std::shared_ptr<AssetPackageFile>;
};
#undef _CRT_SECURE_NO_WARNINGS

#endif // !_ASSET_PACKAGE_FILE_H
