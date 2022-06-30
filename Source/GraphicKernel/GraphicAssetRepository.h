/*****************************************************************//**
 * \file   GraphicAssetRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_ASSET_REPOSITORY_H
#define GRAPHIC_ASSET_REPOSITORY_H

#include <string>
#include <optional>
#include <unordered_map>
#include <any>
#include <cassert>

namespace Enigma::Graphics
{
    class AssetRepository
    {
    public:
        AssetRepository();
        ~AssetRepository();
        AssetRepository(const AssetRepository&) = delete;
        AssetRepository& operator=(const AssetRepository&) = delete;

        void Clear();

        /** Add key value data */
        template <class T> void Add(const std::string& key, const T& value)
        {
            assert(!HasData(key));
            m_dataValues.emplace(key, value);
        }

        /** Remove key value data */
        void Remove(const std::string& key);

        bool HasData(const std::string& key);

        /** Get data, assert if key not found */
        template <class T> T Get(const std::string& key)
        {
            assert(HasData(key));
            return std::any_cast<T>(m_dataValues[key]);
        }

        /** Try get data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryGetValue(const std::string& key)
        {
            if (!HasData(key)) return std::nullopt;
            return std::any_cast<T>(m_dataValues[key]);
        }

    private:
        using DataValueMap = std::unordered_map<std::string, std::any>;
        DataValueMap m_dataValues;
    };

}

#endif // GRAPHIC_ASSET_REPOSITORY_H
