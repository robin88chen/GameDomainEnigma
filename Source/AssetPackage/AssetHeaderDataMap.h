/********************************************************************
 * \file   AssetHeaderDataMap.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _ASSET_HEADER_DATA_MAP_H
#define _ASSET_HEADER_DATA_MAP_H

#include "AssetErrors.h"
#include <string>
#include <unordered_map>
#include <optional>

namespace Enigma::AssetPackage
{
    using error = std::error_code;
    class AssetHeaderDataMap
    {
    public:
        struct AssetHeaderData
        {
            std::string m_name;
            unsigned int m_version;
            unsigned int m_size;
            unsigned int m_orgSize;
            unsigned int m_offset;
            unsigned int m_crc;
            AssetHeaderData() : m_name{ "" },
                m_version{ 0 }, m_size{ 0 }, m_orgSize{ 0 }, m_offset{ 0 }, m_crc{ 0 } {};
        };
    public:
        AssetHeaderDataMap();
        AssetHeaderDataMap(const AssetHeaderDataMap&) = delete;
        AssetHeaderDataMap(AssetHeaderDataMap&&) = delete;
        ~AssetHeaderDataMap();

        AssetHeaderDataMap& operator=(const AssetHeaderDataMap&) = delete;
        AssetHeaderDataMap& operator=(AssetHeaderDataMap&&) = delete;

        error InsertHeaderData(const AssetHeaderData& header);
        error RemoveHeaderData(const std::string& name);

        bool HasAssetKey(const std::string& name) const;

        void RepackContentOffsets(const unsigned int content_size, const unsigned int base_offset);

        std::optional<AssetHeaderData> TryGetHeaderData(const std::string& name);

        size_t CalcHeaderDataMapBytes() const;

        size_t GetTotalDataCount() const { return m_headerDataMap.size(); };

        std::vector<char> ExportToByteBuffer();
        std::error_code ImportFromByteBuffer(const std::vector<char>& buff);

    private:
        std::unordered_map<std::string, AssetHeaderData> m_headerDataMap;
    };

};

#endif // !_ASSET_HEADER_DATA_MAP_H
