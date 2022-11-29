/*********************************************************************
 * \file   GenericDto.h
 * \brief  一般化的 DTO 物件，存放屬性與資料組，特化後的DTO轉給工廠建立物件
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef GENERIC_DTO_H
#define GENERIC_DTO_H

#include "FactoryDesc.h"
#include "Frameworks/ruid.h"
#include <string>
#include <any>
#include <optional>
#include <unordered_map>
#include <cassert>

namespace Enigma::Engine
{
    class GenericDto
    {
    public:
        using AttributeValues = std::unordered_map<std::string, std::any>;

    public:
        GenericDto();
        ~GenericDto();
        GenericDto(const GenericDto& c) = default;
        GenericDto(GenericDto&& c) = default;
        GenericDto& operator=(const GenericDto& c) = default;
        GenericDto& operator=(GenericDto&& c) = default;

        bool operator==(const GenericDto&) const;

        const Frameworks::Ruid& GetId() const { return m_ruid; }

        bool IsTopLevel() const;
        void AsTopLevel(bool is_top);

        bool IsEmpty() const { return m_values.empty(); }

        /** Add or Update key value data */
        template <class T> void AddOrUpdate(const std::string& attribute, const T& value)
        {
            m_values.insert_or_assign(attribute, value);
        }

        /** add Rtti */
        void AddRtti(const FactoryDesc& rtti);
        FactoryDesc GetRtti() const;

        /** Remove key value data */
        void Remove(const std::string& attribute);

        bool HasValue(const std::string& attribute) const;

        /** Get data, assert if key not found */
        template <class T> T Get(const std::string& attribute)
        {
            assert(HasValue(attribute));
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Get data, assert if key not found */
        template <class T> T Get(const std::string& attribute) const
        {
            assert(HasValue(attribute));
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Try get data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryGetValue(const std::string& attribute) const
        {
            if (!HasValue(attribute)) return std::nullopt;
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Try get data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> TryGetValue(const std::string& attribute)
        {
            if (!HasValue(attribute)) return std::nullopt;
            return std::any_cast<T>(m_values.at(attribute));
        }

        AttributeValues::iterator begin() { return m_values.begin(); }
        AttributeValues::const_iterator begin() const { return m_values.begin(); }
        AttributeValues::iterator end() { return m_values.end(); }
        AttributeValues::const_iterator end() const { return m_values.end(); }

    private:
        Frameworks::Ruid m_ruid; // run-time uniform id
        AttributeValues m_values;
    };
}

#endif // GENERIC_DTO_H
