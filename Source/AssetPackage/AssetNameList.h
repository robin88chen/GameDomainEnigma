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
#include <vector>

namespace Enigma::AssetPackage
{
    class AssetNameList
    {
    public:
        constexpr static unsigned int INVALID_NAME_IDX = 0xffffffff;

    public:
        AssetNameList();
        AssetNameList(const AssetNameList&) = delete;
        AssetNameList(AssetNameList&&) = delete;
        ~AssetNameList();

        AssetNameList& operator=(const AssetNameList&) = delete;
        AssetNameList& operator=(AssetNameList&&) = delete;

        const std::string& GetAssetName(unsigned int name_idx);
        unsigned int SearchAssetName(const std::string& name);  // note : very slow
        unsigned int AppendAssetName(const std::string& name);  // return : name_id

        size_t GetNameCount() const { return m_assetNames.size(); };
        const std::vector<std::string>& GetAssetNameArray();

        size_t CalcNameListDataBytes() const;

        std::vector<char> ExportToByteBuffer();
        void ImportFromByteBuffer(const std::vector<char>& buff);

    private:
        std::vector<std::string> m_assetNames;
    };
};

#endif // !_ASSET_NAME_LIST_H
