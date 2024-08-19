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

        const Frameworks::Ruid& ruid() const { return m_ruid; }

        bool isTopLevel() const;
        void asTopLevel(bool is_top);

        bool isEmpty() const { return m_values.empty(); }

        /** Add or Update key value data */
        template <class T> void addOrUpdate(const std::string& attribute, const T& value)
        {
            m_values.insert_or_assign(attribute, value);
        }

        /** add Rtti */
        void addRtti(const FactoryDesc& rtti);
        FactoryDesc getRtti() const;

        /** add Name */
        void addName(const std::string& name);
        std::string getName() const;

        //GenericPolicyConverter GetPolicyConverter() const;
        //void SetPolicyConverter(GenericPolicyConverter converter);

        /** Remove key value data */
        void remove(const std::string& attribute);

        bool hasValue(const std::string& attribute) const;

        /** Get data, assert if key not found */
        template <class T> T get(const std::string& attribute)
        {
            assert(hasValue(attribute));
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Get data, assert if key not found */
        template <class T> T get(const std::string& attribute) const
        {
            assert(hasValue(attribute));
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Try get data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> tryGetValue(const std::string& attribute) const
        {
            if (!hasValue(attribute)) return std::nullopt;
            return std::any_cast<T>(m_values.at(attribute));
        }

        /** Try get data, return nullopt if key not found.
         *  return value is the copy.
         **/
        template <class T> std::optional<T> tryGetValue(const std::string& attribute)
        {
            if (!hasValue(attribute)) return std::nullopt;
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
    using GenericDtoCollection = std::vector<GenericDto>;
}

#endif // GENERIC_DTO_H
