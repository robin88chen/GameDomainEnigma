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
        AssetPackageFile();
        AssetPackageFile(const AssetPackageFile&) = delete;
        AssetPackageFile(AssetPackageFile&&) = delete;
        ~AssetPackageFile();
        AssetPackageFile& operator=(const AssetPackageFile&) = delete;
        AssetPackageFile& operator=(AssetPackageFile&&) = delete;

        const std::string& GetBaseFilename() { return m_baseFilename; };
        error CreateNewPackage(const std::string& basefilename);
        error OpenPackage(const std::string& basefilename);

        error AddAssetFile(const std::string& file_path, const std::string& asset_key, unsigned int version);
        error AddAssetMemory(const std::vector<char>& buff, const std::string& asset_key, unsigned int version);
        error TryRetrieveAssetToFile(const std::string& file_path, const std::string& asset_key);
        std::optional<std::vector<char>> TryRetrieveAssetToMemory(const std::string& asset_key);
        unsigned int GetAssetOriginalSize(const std::string& asset_key);
        time_t GetAssetTimeStamp(const std::string& asset_key);

        error RemoveAsset(const std::string& asset_key);

        const std::unique_ptr<AssetNameList>& GetAssetNameList() { return m_nameList; };
        std::optional<AssetHeaderDataMap::AssetHeaderData> TryGetAssetHeaderData(const std::string& asset_key);
    private:
        void ResetPackage();

        void SaveHeaderFile();
        void ReadHeaderFile();

        std::tuple<std::vector<char>, unsigned int> ReadBundleContent(unsigned int offset, unsigned int content_size);

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
};
#undef _CRT_SECURE_NO_WARNINGS

#endif // !_ASSET_PACKAGE_FILE_H
