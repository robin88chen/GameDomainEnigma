/********************************************************************
 * \file   AssetNameList.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _ASSET_NAME_LIST_H
#define _ASSET_NAME_LIST_H

#include <string>
#include <unordered_set>
#include <system_error>

namespace Enigma::AssetPackage
{
    using error = std::error_code;
    class AssetNameList
    {
    public:
        AssetNameList();
        AssetNameList(const AssetNameList&) = delete;
        AssetNameList(AssetNameList&&) = delete;
        ~AssetNameList();

        AssetNameList& operator=(const AssetNameList&) = delete;
        AssetNameList& operator=(AssetNameList&&) = delete;

        bool HasAssetName(const std::string& name);
        error AppendAssetName(const std::string& name);
        error RemoveAssetName(const std::string& name);

        size_t GetNameCount() const { return m_assetNames.size(); };

        size_t CalcNameListDataBytes() const;

        std::unordered_set<std::string> GetAssetNames() const { return m_assetNames; }

        std::vector<char> ExportToByteBuffer() const;
        error ImportFromByteBuffer(const std::vector<char>& buff);

    private:
        std::unordered_set<std::string> m_assetNames;
    };
};

#endif // !_ASSET_NAME_LIST_H
