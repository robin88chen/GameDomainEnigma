/*********************************************************************
 * \file   Contract.h
 * \brief  合約物件，存放屬性與資料組，給合約工廠建立物件
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CONTRACT_H
#define CONTRACT_H

#include "FactoryDesc.h"
#include "Platforms/ruid.h"
#include <string>
#include <any>
#include <optional>
#include <unordered_map>
#include <cassert>

namespace Enigma::Engine
{
    class Contract
    {
    public:
        using AttributeValues = std::unordered_map<std::string, std::any>;

    public:
        Contract();
        ~Contract();
        Contract(const Contract& c) = default;
        Contract(Contract&& c) = default;
        Contract& operator=(const Contract& c) = default;
        Contract& operator=(Contract&& c) = default;

        bool operator==(const Contract&) const;

        const Platforms::Ruid& GetId() { return m_ruid; }

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
        Platforms::Ruid m_ruid; // run-time uniform id
        AttributeValues m_values;
    };
}

#endif // CONTRACT_H
