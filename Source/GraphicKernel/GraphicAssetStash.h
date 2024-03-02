/*****************************************************************//**
 * \file   GraphicAssetStash.h
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
#include <mutex>

namespace Enigma::Graphics
{
    class AssetStash
    {
    public:
        AssetStash();
        ~AssetStash();
        AssetStash(const AssetStash&) = delete;
        AssetStash(AssetStash&&) = delete;
        AssetStash& operator=(const AssetStash&) = delete;
        AssetStash& operator=(AssetStash&&) = delete;

        void clear();

        /** Add key value data */
        template <class T> void Add(const std::string& key, const T& value)
        {
            std::lock_guard locker{ m_lock };
            assert(!HasData(key));
            m_dataValues.emplace(key, value);
        }

        /** Remove key value data */
        void remove(const std::string& key);

        bool HasData(const std::string& key);

        /** Get data, assert if key not found */
        template <class T> T get(const std::string& key)
        {
            std::lock_guard locker{ m_lock };
            assert(HasData(key));
            T value = std::any_cast<T>(m_dataValues[key]);
            remove(key);
            return value;
        }

        /** Try find data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryFindValue(const std::string& key)
        {
            std::lock_guard locker{ m_lock };
            if (!HasData(key)) return std::nullopt;
            return std::any_cast<T>(m_dataValues[key]);
        }

    private:
        using DataValueMap = std::unordered_map<std::string, std::any>;
        DataValueMap m_dataValues;
        std::recursive_mutex m_lock;
    };

}

#endif // GRAPHIC_ASSET_REPOSITORY_H
